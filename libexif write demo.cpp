#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#include <conio.h>

#include <string>
#include <assert.h>
#include <memory>
#include <functional>

#include <DC_File.h>

#include <libexif/exif-data.h>

void dosth() {}

#define FILE_BYTE_ORDER EXIF_BYTE_ORDER_MOTOROLA

static const unsigned int image_data_offset = 20;
static const unsigned char exif_header[] = { 0xff, 0xd8, 0xff, 0xe1 };
static const unsigned int exif_header_size = sizeof(exif_header);
static const std::string exif_header_str(reinterpret_cast<const char*>(exif_header), 4);

ExifEntry* init_tag(ExifData *exif, ExifIfd ifd, ExifTag tag) {
	ExifEntry *entry;
	if (!((entry = exif_content_get_entry(exif->ifd[ifd], tag)))) {
		entry = exif_entry_new();
		if (entry == 0)
			throw std::bad_alloc();
		entry->tag = tag;
		exif_content_add_entry(exif->ifd[ifd], entry);
		exif_entry_initialize(entry, tag);
		exif_entry_unref(entry);
	}
	return entry;
}

ExifEntry* create_tag(ExifData *exif, ExifIfd ifd, ExifTag tag, int len) {
	void *buf;
	ExifEntry *entry;
	ExifMem *mem = exif_mem_new_default();
	if (mem == 0)
		throw std::bad_alloc();
	entry = exif_entry_new_mem(mem);
	if (entry == 0)
		throw std::bad_alloc();
	buf = exif_mem_alloc(mem, len);
	if (buf == 0)
		throw std::bad_alloc();

	entry->data = reinterpret_cast<unsigned char*>(buf);
	entry->size = len;
	entry->tag = tag;
	entry->components = len;
	entry->format = EXIF_FORMAT_UNDEFINED;

	exif_content_add_entry(exif->ifd[ifd], entry);

	exif_mem_unref(mem);
	exif_entry_unref(entry);

	return entry;
}

bool write_exif(
	std::string& output,
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
		ExifEntry *entry;
		std::unique_ptr<ExifData, std::function<void(ExifData*)>> exif(
			exif_data_new(),
			[](ExifData* p) {
			dosth();
			exif_data_unref(p);
			//exif_data_free(p);
		});

		exif_data_set_option(exif.get(), EXIF_DATA_OPTION_FOLLOW_SPECIFICATION);
		exif_data_set_data_type(exif.get(), EXIF_DATA_TYPE_COMPRESSED);
		exif_data_set_byte_order(exif.get(), FILE_BYTE_ORDER);
				
		entry = create_tag(exif.get(), EXIF_IFD_EXIF, EXIF_TAG_DATE_TIME_ORIGINAL, DateTime.size());
		entry->format = EXIF_FORMAT_ASCII;
		memcpy(entry->data, DateTime.data(), DateTime.size());

		entry = create_tag(exif.get(), EXIF_IFD_0, EXIF_TAG_SOFTWARE, Software.size());
		entry->format = EXIF_FORMAT_ASCII;
		memcpy(entry->data, Software.data(), Software.size());

		entry = create_tag(exif.get(), EXIF_IFD_0, EXIF_TAG_MAKE, Make.size());
		entry->format = EXIF_FORMAT_ASCII;
		memcpy(entry->data, Make.data(), Make.size());

		entry = create_tag(exif.get(), EXIF_IFD_0, EXIF_TAG_MODEL, Model.size());
		entry->format = EXIF_FORMAT_ASCII;
		memcpy(entry->data, Model.data(), Model.size());

		entry = create_tag(exif.get(), EXIF_IFD_EXIF, EXIF_TAG_LENS_MODEL, Camera.size());
		entry->format = EXIF_FORMAT_ASCII;
		memcpy(entry->data, Camera.data(), Camera.size());

		entry = init_tag(exif.get(), EXIF_IFD_EXIF, EXIF_TAG_FOCAL_LENGTH);
		entry->format = EXIF_FORMAT_RATIONAL;
		exif_set_rational(entry->data, FILE_BYTE_ORDER, { (ExifLong)(FocalLength * 1000) ,(ExifLong)1000 });

		entry = init_tag(exif.get(), EXIF_IFD_EXIF, EXIF_TAG_FOCAL_LENGTH_IN_35MM_FILM);
		entry->format = EXIF_FORMAT_SHORT;
		exif_set_short(entry->data, FILE_BYTE_ORDER, FocalLengthIn35mm);

		entry = init_tag(exif.get(), EXIF_IFD_EXIF, EXIF_TAG_EXPOSURE_TIME);
		entry->format = EXIF_FORMAT_RATIONAL;
		exif_set_rational(entry->data, FILE_BYTE_ORDER, { (ExifLong)(ExposureTime * 1000) ,(ExifLong)1000 });

		entry = init_tag(exif.get(), EXIF_IFD_EXIF, EXIF_TAG_FNUMBER);
		entry->format = EXIF_FORMAT_RATIONAL;
		exif_set_rational(entry->data, FILE_BYTE_ORDER, { (ExifLong)(FNumber * 1000) ,(ExifLong)1000 });

		entry = init_tag(exif.get(), EXIF_IFD_EXIF, EXIF_TAG_ISO);
		entry->format = EXIF_FORMAT_SHORT;
		exif_set_short(entry->data, FILE_BYTE_ORDER, ISOSpeedRatings);

		entry = init_tag(exif.get(), EXIF_IFD_EXIF, EXIF_TAG_EXPOSURE_PROGRAM);
		entry->format = EXIF_FORMAT_SHORT;
		exif_set_short(entry->data, FILE_BYTE_ORDER, ExposureProgram);

		entry = init_tag(exif.get(), EXIF_IFD_EXIF, EXIF_TAG_FLASH);
		entry->format = EXIF_FORMAT_SHORT;
		exif_set_short(entry->data, FILE_BYTE_ORDER, Flash);

		entry = init_tag(exif.get(), EXIF_IFD_0, EXIF_TAG_IMAGE_WIDTH);
		exif_set_long(entry->data, FILE_BYTE_ORDER, ImageWidth);

		entry = init_tag(exif.get(), EXIF_IFD_0, EXIF_TAG_IMAGE_LENGTH);
		exif_set_long(entry->data, FILE_BYTE_ORDER, ImageHeight);

		entry = create_tag(exif.get(), EXIF_IFD_GPS, EXIF_TAG_LATITUDE, exif_format_get_size(EXIF_FORMAT_RATIONAL));
		entry->format = EXIF_FORMAT_RATIONAL;
		exif_set_rational(entry->data, FILE_BYTE_ORDER, { (ExifLong)(Latitude * 1000) ,(ExifLong)1000 });

		entry = create_tag(exif.get(), EXIF_IFD_GPS, EXIF_TAG_LONGITUDE, exif_format_get_size(EXIF_FORMAT_RATIONAL));
		entry->format = EXIF_FORMAT_RATIONAL;
		exif_set_rational(entry->data, FILE_BYTE_ORDER, { (ExifLong)(Longitude * 1000) ,(ExifLong)1000 });

		entry = create_tag(exif.get(), EXIF_IFD_GPS, EXIF_TAG_ALTITUDE, exif_format_get_size(EXIF_FORMAT_RATIONAL));
		entry->format = EXIF_FORMAT_RATIONAL;
		exif_set_rational(entry->data, FILE_BYTE_ORDER, { (ExifLong)(Altitude * 1000) ,(ExifLong)1000 });

		unsigned char *exif_data = nullptr;
		unsigned int exif_data_len = 0;
		exif_data_save_data(exif.get(), &exif_data, &exif_data_len);
		if (exif_data == nullptr)
			throw std::bad_alloc();

		std::string exif_data_str(reinterpret_cast<char*>(exif_data), exif_data_len);

		output.clear();
		output += exif_header_str;
		output += (exif_data_len + 2) >> 8;
		output += (exif_data_len + 2) & 0xff;
		output += exif_data_str;
		output += std::string(image_data.data() + image_data_offset, image_data.size() - image_data_offset);

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
		std::string result;
		write_exif(result, img, "2017:12:08 16:18:45", "HEIF Utility", "Apple", "iPhone SE 2 runs Android", "iPhone SE 2 back camera 3.99mm f/1.8", 3.99, 28, 0.004, 1.8, 50, 1, 1, 3024, 4032, 23.1541, 113.324, 8848, 0);
		DC::File::write<DC::File::binary>("out.jpg", result);
		dosth();
	}
	_CrtDumpMemoryLeaks();
}