using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using Newtonsoft.Json;
//需要确保文件为 UTF8 with BOM 编码

namespace ZeldaData
{
    class Program
    {
        static void Main(string[] args)
        {
            string md, content, type;
            Console.Write("输入md文件名:");
            //md = Console.ReadLine();
            md = "E:\\OneDrive\\杂项\\Zelda塞尔达项目\\md\\Monsters.md";           
            content = File.ReadAllText(md, Encoding.UTF8);

            //bool ShouldClean = false;
            bool ShouldClean = true;
            if (ShouldClean)
            {
                int pos = content.IndexOf("![", 0);
                while (pos != -1)
                {
                    content = content.Remove(pos - 1, 1);
                    pos--;
                    pos = content.IndexOf("\n\n", pos);
                    content = content.Remove(pos, 1);
                    pos = content.IndexOf("![", pos);
                }
            }

            type = content.Substring(content.IndexOf('#', 0) + 2, content.IndexOf('\n', 0) - 2 - content.IndexOf('#', 0));
            var bgfrs = Util.search_all(content, "##");
            var edfrs = Util.search_all(content, "\n\n");
            var items = Util.get_items_from_substrings(Util.get_item_substrings(content, bgfrs, edfrs));
            var look = JsonConvert.SerializeObject(items, Formatting.Indented);
            File.WriteAllText("E:\\OneDrive\\杂项\\Zelda塞尔达项目\\Monsters.json", look, Encoding.UTF8);
        }
    }
}
