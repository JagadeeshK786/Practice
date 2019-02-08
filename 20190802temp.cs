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
using System.Net.Security;
using System.Net.Sockets;
using System.Security.Cryptography.X509Certificates;
using System.Text;

namespace ConsoleApp1
{
    class Program
    {
        static void Main(string[] args)
        {
            try
            {
                TcpClient sslClient = new TcpClient();
                sslClient.Connect("127.0.0.1", 8080);

                SslStream sslStream = new SslStream(sslClient.GetStream(), false, (object sender, X509Certificate certificate, X509Chain chain, SslPolicyErrors sslPolicyErrors) => { return true; });

                sslStream.AuthenticateAsClient("");
                var writeMe = Encoding.ASCII.GetBytes("GET /zzz/zzz?name=value HTTP/1.1\r\nUser-Agent: isme\r\n\r\n");
                sslStream.Write(writeMe, 0, writeMe.Length);
                byte[] readMe = new byte[100];
                sslStream.Close();
                sslStream.Read(readMe, 0, readMe.Length);
                sslStream.Read(readMe, 0, readMe.Length);
                Console.Write("");
            }
            catch (System.Exception ex)
            {
                Console.WriteLine(ex.Message);
            }
        }
    }
}
