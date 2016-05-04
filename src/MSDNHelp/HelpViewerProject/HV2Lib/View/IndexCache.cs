using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
#if VSDLL
using Microsoft.VisualStudio.Help.Runtime;
#else
using Windows.Help.Runtime;
#endif

namespace HVProject.HV2Lib.View
{

    /// <summary>
    /// Simple cache to buffer and smooth the Visible Index control data fetches 
    /// </summary>

    public class IndexCache  
    {
        List<int> _index;            //indirect index into other arrays
        List<int> _topicCount;       //speed up to store this insted of fetching the topic list eachtime
        List<Keyword> _cache;
        const int MaxItems = 200;
        
        public IndexCache()
        {
            Clear();
        }

        public void Clear()
        {
            _index = new List<int>();
            _topicCount = new List<int>();
            _cache = new List<Keyword>();
        }

        public void Add(int index, Keyword keyword, int topicCount)
        {
            if (_cache.Count > MaxItems)
                Clear();
            _index.Add(index);
            _cache.Add(keyword);
            _topicCount.Add(topicCount);
        }

        public void Add(int index, Keyword keyword)   //call this if not interested in recalling topic count
        {
            this.Add(index, keyword, 0);
        }

        public Keyword GetKW(int index, out int topicCount)
        {
            topicCount = 0;
            for (int i = 0; i < _index.Count; i++)
            {
                if (index == _index[i])
                {
                    topicCount = _topicCount[i];
                    return _cache[i];
                }
            }
            return null;
        }

        public Keyword GetKW(int index)  //call this if not interested in recalling topic count
        {
            for (int i = 0; i < _index.Count; i++)
            {
                if (index == _index[i])
                    return _cache[i];
            }
            return null;
        }
		public int GetSelect(int index)
		{
            for (int i = 0; i < _index.Count; i++)
            {
                if (index == _index[i])
                    return i;
            }
			return -1;
		}
    }

}
