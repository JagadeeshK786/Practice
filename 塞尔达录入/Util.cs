using System;
using System.Collections.Generic;
using System.Text;

namespace ZeldaData
{
    class Util
    {
        static public string image_weapons_url = "https://starryedge.com/ZeldaPS/Data/Image/Weapons/",
            page_weapons_url = "https://starryedge.com/ZeldaPS/Data/Page/Weapons/";

        public static List<int> search_all(string str, string findme)
        {
            List<int> rv = new List<int>();
            int pos = str.IndexOf(findme, 0);
            while (pos != -1)
            {
                rv.Add(pos);
                pos = str.IndexOf(findme, pos + 1);
            }
            return rv;
        }

        public static int search_time(string str, string findme, int times)
        {
            int pos = -1;
            for (int i = 0; i < times; i++)
            {
                pos = str.IndexOf(findme, pos + 1);
                if (pos == -1)
                    break;
            }
            return pos;
        }

        public static List<string> get_item_substrings(string str, List<int> bglist, List<int> edlist)
        {
            if (bglist.Count != edlist.Count)
                throw new Exception();
            List<string> substrings = new List<string>();
            for (int i = 0; i < bglist.Count; i++)
                substrings.Add(str.Substring(bglist[i], edlist[i] - bglist[i] + 1));
            return substrings;
        }

        public static kvpair GetKvpair(string line, char suxx)
        {
            return new kvpair(line.Substring(0, line.IndexOf(suxx)), line.Substring(line.IndexOf(suxx) + 1, line.Length - 1 - line.IndexOf(suxx)));
        }

        public static List<item> get_items_from_substrings(List<string> substrings)
        {
            List<item> rv = new List<item>();
            foreach (string p in substrings)
            {
                item it = new item();
                it.ChineseName = p.Substring(search_time(p, " ", 2) + 1, p.IndexOf('!') - search_time(p, " ", 2) - 1);
                rv.Add(it);
                it.Number = p.Substring(p.IndexOf(' ') + 1, search_time(p, " ", 2) - p.IndexOf(' ') - 1);
                it.Image = image_weapons_url + it.Number.Replace('/', '_') + ".png";
                //it.ChineseIntro = page_weapons_url + it.Number.Replace('/', '_');
                it.Description = new description_type[1] { new description_type() };
                it.Description[0].Language = "CN";
                var nfres = search_all(p, "\n");
                if (nfres.Count == 0 || nfres.Count == 1)
                    continue;
                it.Description[0].Fields = new kvpair[nfres.Count - 1];
                for (int i = 0; i < nfres.Count - 1; i++)
                {
                    if (p[search_time(p, "\n", i + 1) + 1] != '+')
                        throw new Exception();
                    it.Description[0].Fields[i] =
                        GetKvpair(p.Substring(search_time(p, "\n", i + 1) + 3, search_time(p, "\n", i + 2) - 3 - search_time(p, "\n", i + 1)), '：');
                }
            }
            return rv;
        }

        public static string toBase64(string str)
        {
            byte[] temp = System.Text.Encoding.Default.GetBytes(str);
            return Convert.ToBase64String(temp);
        }

        public static string fromBase64(string str)
        {
            byte[] temp = Convert.FromBase64String(str);
            return System.Text.Encoding.Default.GetString(temp);
        }
    }

    class item
    {
        public string ChineseName
        {
            //get { return Util.fromBase64(chinesename); }
            get { return chinesename; }
            set { chinesename = Util.toBase64(value); }
        }
        private string chinesename;
        public string Image { get; set; }
        //public string ChineseIntro { get; set; }
        public string Number { get; set; }
        public description_type[] Description { get; set; }
    }

    class description_type
    {
        public string Language;
        public kvpair[] Fields;
    }

    class kvpair
    {
        public kvpair(string n, string v)
        {
            Name = n;
            Value = v;
        }

        private string name = "", ivalue = "";

        public string Name
        {
            get { return name; }
            //set { name = value; }
            set { name = Util.toBase64(value); }
        }

        public string Value
        {
            get { return ivalue; }
            //set { ivalue = value; }
            set { ivalue = Util.toBase64(value); }
        }
    }
}
