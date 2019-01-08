/*using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Security.Cryptography;
using System.Text;

namespace ConsoleApp1
{
    class Program
    {
        static void Main(string[] args)
        {
            var folder = @"C:\Users\liuzi\Desktop\zelda_botw_map - 副本\Content\Map";
            DirectoryInfo d = new DirectoryInfo(folder);
            FileInfo[] files = d.GetFiles();
            Dictionary<string, List<string>> hashinfo = new Dictionary<string, List<string>>();
            using (var md5 = MD5.Create())
            {
                foreach (var file in files)
                {
                    using (var stream = File.OpenRead(file.FullName))
                    {
                        var hashOfFile = ToHex(md5.ComputeHash(stream), false);
                        if (!hashinfo.ContainsKey(hashOfFile))
                            hashinfo[hashOfFile] = new List<string>();
                        hashinfo[hashOfFile].Add(file.Name);
                    }
                }
            }
            List<List<string>> result = new List<List<string>>();
            foreach (var pair in hashinfo)
            {
                if (pair.Value.Count != 1)
                    result.Add(pair.Value);
            }
            Console.WriteLine("有" + result.Count + "个hash被重复");
            for (int i = 0; i < result.Count; i++)
            {
                Console.Write(i + "  ");
                foreach (var element in result[i])
                {
                    Console.Write(element + " ");
                }
                Console.Write("\r\n");
            }
            Console.ReadKey();
        }

        public static string ToHex(byte[] bytes, bool upperCase)
        {
            StringBuilder result = new StringBuilder(bytes.Length * 2);

            for (int i = 0; i < bytes.Length; i++)
                result.Append(bytes[i].ToString(upperCase ? "X2" : "x2"));

            return result.ToString();
        }
    }
}
*/
using System;
using System.Collections.Generic;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;

namespace ConsoleApp1
{
    class Program
    {
        static void Main(string[] args)
        {
            var folder = @"C:\Users\liuzi\Documents\GitHub\Zelda-Project-Server\In-App Map\Content\Map";
            ToJPG(folder);
        }
        static public void ToJPG(string folder)
        {
            var files = System.IO.Directory.GetFiles(folder);
            foreach (string file in files)
            {
                string extension = System.IO.Path.GetExtension(file);
                if (extension == ".png")
                {
                    string name = System.IO.Path.GetFileNameWithoutExtension(file);
                    string path = System.IO.Path.GetDirectoryName(file);
                    using (System.Drawing.Image png = System.Drawing.Image.FromFile(file))
                    {
                        using (var b = new Bitmap(png.Width, png.Height))
                        {
                            b.SetResolution(png.HorizontalResolution, png.VerticalResolution);
                            using (var g = Graphics.FromImage(b))
                            {
                                g.Clear(Color.FromArgb(220, 220, 220));
                                g.DrawImageUnscaled(png, 0, 0);
                            }
                            b.Save(path + @"/" + name + ".jpg", System.Drawing.Imaging.ImageFormat.Jpeg);
                        }
                    }
                }
            }
            foreach (string file in files)
            {
                File.Delete(file);
            }
        }
    }
}
