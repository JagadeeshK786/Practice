#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#include <conio.h>

#include <liuzianglib.h>
#include <DC_File.h>
#include <DC_STR.h>

#include <stdio.h>
#include <vector>

#include <windows.h>
#include <gdiplus.h>
#include <Shlwapi.h>
#include <atlbase.h>
#pragma comment( lib, "gdiplus" )
#pragma comment( lib, "Shlwapi.lib" )

using namespace Gdiplus;

static const uint32_t sizeof_exif_byte = 1;
static const uint32_t sizeof_exif_long = 4;
static const uint32_t sizeof_exif_rational = 4 * 2;
static const uint32_t sizeof_exif_short = 2;
static const uint32_t sizeof_exif_slong = 4;
static const uint32_t sizeof_exif_srational = 4 * 2;

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid) {
	UINT  num = 0;          // number of image encoders
	UINT  size = 0;         // size of the image encoder array in bytes
	ImageCodecInfo* pImageCodecInfo = NULL;
	GetImageEncodersSize(&num, &size);
	if (size == 0)
		return -1;  // Failure
	pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
	if (pImageCodecInfo == NULL)
		return -1;  // Failure
	GetImageEncoders(num, size, pImageCodecInfo);
	for (UINT j = 0; j < num; ++j)
	{
		if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;  // Success
		}
	}
	free(pImageCodecInfo);
	return 0;
}

struct load_gdip {
	load_gdip() {
		GdiplusStartupInput gdiplusStartupInput;
		if (GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL) != Status::Ok)
			throw std::runtime_error("GDI+ startup error");
	}

	~load_gdip() {
		GdiplusShutdown(gdiplusToken);
	}

	load_gdip(const load_gdip&) = delete;

private:
	ULONG_PTR gdiplusToken;
};

class mem_ptr {
public:
	mem_ptr() {}

	mem_ptr(DC::size_t size) :ptr(malloc(size)) {
		if (ptr == 0)
			throw std::bad_alloc();
	}

	mem_ptr(const mem_ptr&) = delete;

	mem_ptr(mem_ptr&& input) :ptr(input.ptr) {
		input.ptr = nullptr;
	}

	~mem_ptr() {
		if (ptr != nullptr)
			free(ptr);
	}

	mem_ptr& operator=(const mem_ptr&) = delete;

	void* set(DC::size_t size) {
		this->clear();
		ptr = malloc(size);

		if (ptr == 0)
			throw std::bad_alloc();

		return ptr;
	}

	void* get()const {
		return	ptr;
	}

	void clear() {
		if (ptr != nullptr)
			free(ptr);
	}

private:
	void * ptr = nullptr;
};

template <typename ExifType, typename ...ARGS>
void* make_buffer(std::vector<mem_ptr>& pool, ARGS&& ...args) {
	using real_type = std::decay_t<ExifType>;
	auto args_vec = DC::GetArgs(std::forward<ARGS>(args)...);
	pool.emplace_back(sizeof(real_type) * args_vec.size());
	auto array = reinterpret_cast<real_type*>(pool.rbegin()->get());
	for (DC::size_t i = 0; i < args_vec.size(); i++)
		array[i] = args_vec[i].get<real_type>();
	return pool.rbegin()->get();
}

struct alignas(8) exif_rational {
	unsigned long numerator, denominator;
};

bool write_exif(
	const std::string& output_filename,
	const std::string& image_data,
	const std::string& DateTime,
	const std::string& Software,
	const std::string& Make,
	const std::string& Model,
	const std::string& Camera,
	const double& FocalLength,
	const unsigned short& FocalLengthIn35mm,
	const double& ExposureTime,
	const double& FNumber,
	const unsigned short& ISOSpeedRatings,
	const unsigned short& ExposureProgram,
	const char& Flash,
	const unsigned int& ImageHeight,
	const unsigned int& ImageWidth,
	const double& Latitude,
	const double& Longitude,
	const double& Altitude,
	const double& DOP
)noexcept {
	try {
		//检查编译环境类型长度是否符合预期
		static_assert(sizeof(char) == sizeof_exif_byte &&
			sizeof(unsigned long) == sizeof_exif_long &&
			sizeof(exif_rational) == sizeof_exif_rational &&
			sizeof(unsigned short) == sizeof_exif_short &&
			sizeof(signed long) == sizeof_exif_slong,
			"the type length does not match the expected value");

		load_gdip gdip;
		CLSID  clsid;
		GetEncoderClsid(L"image/jpeg", &clsid);

		CComPtr<IStream> image_data_stream(SHCreateMemStream(reinterpret_cast<const BYTE*>(image_data.data()), image_data.size()));

		std::unique_ptr<Bitmap> bitmap(Gdiplus::Bitmap::FromStream(image_data_stream));
		std::vector<mem_ptr> memory_pool;
		memory_pool.reserve(10);
		auto make_buffer_string = [&memory_pool](const std::string& str)->void* {
			memory_pool.emplace_back(str.size() + 1);
			memcpy(memory_pool.rbegin()->get(), str.data(), str.size());
			reinterpret_cast<char*>(memory_pool.rbegin()->get())[str.size()] = 0;
			return memory_pool.rbegin()->get();
		};
		auto make_buffer_rational = [&memory_pool](unsigned long numerator, unsigned long denominator)->void* {
			memory_pool.emplace_back(sizeof_exif_rational);
			*reinterpret_cast<unsigned long*>(memory_pool.rbegin()->get()) = numerator;
			*(reinterpret_cast<unsigned long*>(memory_pool.rbegin()->get()) + 1) = denominator;
			return memory_pool.rbegin()->get();
		};
		auto make_buffer_short = [&memory_pool](unsigned short number)->void* {
			memory_pool.emplace_back(sizeof_exif_short);
			*reinterpret_cast<unsigned short*>(memory_pool.rbegin()->get()) = number;
			return memory_pool.rbegin()->get();
		};
		auto make_buffer_long = [&memory_pool](unsigned long number)->void* {
			memory_pool.emplace_back(sizeof_exif_long);
			*reinterpret_cast<unsigned long*>(memory_pool.rbegin()->get()) = number;
			return memory_pool.rbegin()->get();
		};
		auto make_buffer_byte = [&memory_pool](char value)->void* {
			memory_pool.emplace_back(sizeof_exif_byte);
			*reinterpret_cast<char*>(memory_pool.rbegin()->get()) = value;
			return memory_pool.rbegin()->get();
		};

		PropertyItem propertyItemDateTime;
		propertyItemDateTime.id = 0x9003;
		propertyItemDateTime.length = DateTime.size() + 1;
		propertyItemDateTime.type = PropertyTagTypeASCII;
		propertyItemDateTime.value = make_buffer_string(DateTime);
		bitmap->SetPropertyItem(&propertyItemDateTime);

		PropertyItem propertyItemSoftware;
		propertyItemSoftware.id = 0x0131;
		propertyItemSoftware.length = Software.size() + 1;
		propertyItemSoftware.type = PropertyTagTypeASCII;
		propertyItemSoftware.value = make_buffer_string(Software);
		bitmap->SetPropertyItem(&propertyItemSoftware);

		PropertyItem propertyItemMake;
		propertyItemMake.id = 0x010f;
		propertyItemMake.length = Make.size() + 1;
		propertyItemMake.type = PropertyTagTypeASCII;
		propertyItemMake.value = make_buffer_string(Make);
		bitmap->SetPropertyItem(&propertyItemMake);

		PropertyItem propertyItemModel;
		propertyItemModel.id = 0x0110;
		propertyItemModel.length = Model.size() + 1;
		propertyItemModel.type = PropertyTagTypeASCII;
		propertyItemModel.value = make_buffer_string(Model);
		bitmap->SetPropertyItem(&propertyItemModel);

		PropertyItem propertyItemCamera;
		propertyItemCamera.id = 0xa434;
		propertyItemCamera.length = Camera.size() + 1;
		propertyItemCamera.type = PropertyTagTypeASCII;		
		propertyItemCamera.value = make_buffer_string(Camera);
		bitmap->SetPropertyItem(&propertyItemCamera);

		PropertyItem propertyItemFocalLength;
		propertyItemFocalLength.id = 0x920a;
		propertyItemFocalLength.length = sizeof_exif_rational;
		propertyItemFocalLength.type = PropertyTagTypeRational;
		propertyItemFocalLength.value = make_buffer_rational(FocalLength * 1000, 1000);
		bitmap->SetPropertyItem(&propertyItemFocalLength);
		
		PropertyItem propertyItemFocalLengthIn35mm;
		propertyItemFocalLengthIn35mm.id = 0xa405;
		propertyItemFocalLengthIn35mm.length = sizeof_exif_short;
		propertyItemFocalLengthIn35mm.type = PropertyTagTypeShort;
		propertyItemFocalLengthIn35mm.value = make_buffer_short(FocalLengthIn35mm);
		bitmap->SetPropertyItem(&propertyItemFocalLengthIn35mm);

		PropertyItem propertyItemExposureTime;
		propertyItemExposureTime.id = 0x829a;
		propertyItemExposureTime.length = sizeof_exif_rational;
		propertyItemExposureTime.type = PropertyTagTypeRational;
		propertyItemExposureTime.value = make_buffer_rational(ExposureTime * 1000, 1000);
		bitmap->SetPropertyItem(&propertyItemExposureTime);

		PropertyItem propertyItemFNumber;
		propertyItemFNumber.id = 0x829d;
		propertyItemFNumber.length = sizeof_exif_rational;
		propertyItemFNumber.type = PropertyTagTypeRational;
		propertyItemFNumber.value = make_buffer_rational(FNumber * 1000, 1000);
		bitmap->SetPropertyItem(&propertyItemFNumber);

		PropertyItem propertyItemISOSpeedRatings;
		propertyItemISOSpeedRatings.id = 0x8827;
		propertyItemISOSpeedRatings.length = sizeof_exif_short;
		propertyItemISOSpeedRatings.type = PropertyTagTypeShort;
		propertyItemISOSpeedRatings.value = make_buffer_short(ISOSpeedRatings);
		bitmap->SetPropertyItem(&propertyItemISOSpeedRatings);

		PropertyItem propertyItemExposureProgram;
		propertyItemExposureProgram.id = 0x8822;
		propertyItemExposureProgram.length = sizeof_exif_short;
		propertyItemExposureProgram.type = PropertyTagTypeShort;
		propertyItemExposureProgram.value = make_buffer_short(ExposureProgram);
		bitmap->SetPropertyItem(&propertyItemExposureProgram);

		PropertyItem propertyItemFlash;
		propertyItemFlash.id = 0x9209;
		propertyItemFlash.length = sizeof_exif_byte;
		propertyItemFlash.type = PropertyTagTypeByte;
		propertyItemFlash.value = make_buffer_byte(Flash);
		bitmap->SetPropertyItem(&propertyItemFlash);

		PropertyItem propertyItemImageHeight;
		propertyItemImageHeight.id = PropertyTagImageHeight;
		propertyItemImageHeight.length = sizeof_exif_long;
		propertyItemImageHeight.type = PropertyTagTypeLong;
		propertyItemImageHeight.value = make_buffer_long(ImageHeight);
		bitmap->SetPropertyItem(&propertyItemImageHeight);

		PropertyItem propertyItemImageWidth;
		propertyItemImageWidth.id = PropertyTagImageWidth;
		propertyItemImageWidth.length = sizeof_exif_long;
		propertyItemImageWidth.type = PropertyTagTypeLong;
		propertyItemImageWidth.value = make_buffer_long(ImageWidth);
		bitmap->SetPropertyItem(&propertyItemImageWidth);

		PropertyItem propertyItemLatitude;
		propertyItemLatitude.id = PropertyTagGpsLatitude;
		propertyItemLatitude.length = sizeof_exif_rational * 3;
		propertyItemLatitude.type = PropertyTagTypeRational;
		propertyItemLatitude.value = make_buffer<exif_rational>(memory_pool, exif_rational{ (unsigned long)abs(Latitude * (double)1000000),1000000 }, exif_rational{ 0,0 }, exif_rational{ 0,0 });
		bitmap->SetPropertyItem(&propertyItemLatitude);

		PropertyItem propertyItemLongitude;
		propertyItemLongitude.id = PropertyTagGpsLongitude;
		propertyItemLongitude.length = sizeof_exif_rational * 3;
		propertyItemLongitude.type = PropertyTagTypeRational;
		propertyItemLongitude.value = make_buffer<exif_rational>(memory_pool, exif_rational{ (unsigned long)abs(Longitude * (double)1000000),1000000 }, exif_rational{ 0,0 }, exif_rational{ 0,0 });
		bitmap->SetPropertyItem(&propertyItemLongitude);

		PropertyItem propertyItemLatitudeRef;
		propertyItemLatitudeRef.id = 0x0001;
		propertyItemLatitudeRef.length = sizeof_exif_byte * 2;
		propertyItemLatitudeRef.type = PropertyTagTypeASCII;		
		if (Latitude >= 0)
			propertyItemLatitudeRef.value = make_buffer<char>(memory_pool, 'N', char(0));
		else
			propertyItemLatitudeRef.value = make_buffer<char>(memory_pool, 'S', char(0));
		bitmap->SetPropertyItem(&propertyItemLatitudeRef);

		PropertyItem propertyItemLongitudeRef;
		propertyItemLongitudeRef.id = 0x0003;
		propertyItemLongitudeRef.length = sizeof_exif_byte * 2;
		propertyItemLongitudeRef.type = PropertyTagTypeASCII;
		if (Longitude >= 0)
			propertyItemLongitudeRef.value = make_buffer<char>(memory_pool, 'E', char(0));
		else
			propertyItemLongitudeRef.value = make_buffer<char>(memory_pool, 'W', char(0));
		bitmap->SetPropertyItem(&propertyItemLongitudeRef);

		PropertyItem propertyItemAltitude;
		propertyItemAltitude.id = 0x0006;
		propertyItemAltitude.length = sizeof_exif_rational;
		propertyItemAltitude.type = PropertyTagTypeRational;
		propertyItemAltitude.value = make_buffer_rational(abs(Altitude * 10000), 10000);
		bitmap->SetPropertyItem(&propertyItemAltitude);

		PropertyItem propertyItemAltitudeRef;
		propertyItemAltitudeRef.id = 0x0005;
		propertyItemAltitudeRef.length = sizeof_exif_short;
		propertyItemAltitudeRef.type = PropertyTagTypeShort;
		if (Altitude >= 0)
			propertyItemAltitudeRef.value = make_buffer_short(0);
		else
			propertyItemAltitudeRef.value = make_buffer_short(1);
		bitmap->SetPropertyItem(&propertyItemAltitudeRef);

		PropertyItem propertyItemDOP;
		propertyItemDOP.id = 0x000b;
		propertyItemDOP.length = sizeof_exif_rational;
		propertyItemDOP.type = PropertyTagTypeRational;
		propertyItemDOP.value = make_buffer_rational(abs(DOP * 10000), 10000);
		bitmap->SetPropertyItem(&propertyItemDOP);

		if (bitmap->Save(DC::STR::toType<std::wstring>(output_filename).c_str(), &clsid, NULL) != Ok)
			throw std::exception();

		return true;
	}
	catch (...) {
		return false;
	}
}

int main(int argc, char **argv)
{
	{
		auto img = DC::File::read<DC::File::binary>("test.jpg");
		if (!write_exif("输出.jpg", img, "2017:12:08 16:18:45", "HEIF Utility", "Apple", "iPhone SE 2 runs Android", "iPhone SE 2 back camera 3.99mm f/1.8", 3.99, 28, 0.004, 1.8, 50, 3, 0, 3024, 4032, 42.9256, -88.0943, 8848, 1999.9710))
			abort();
	}
	_CrtDumpMemoryLeaks();
}