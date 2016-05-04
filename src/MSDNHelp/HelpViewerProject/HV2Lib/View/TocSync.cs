using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;
#if VSDLL
using Microsoft.VisualStudio.Help.Runtime;
#else
using Windows.Help.Runtime;
#endif

namespace HVProject.HV2Lib.View
{
    public class TocSync
    {
        private readonly TreeView treeview = null;
        private readonly CatalogRead catalogRead = null;
        private TopicCollection ancestors = null;
        private String lastTopicID = "";

        public TocSync(TreeView treeview)
        {
            this.treeview = treeview;
            catalogRead = new CatalogRead();
        }

        public enum SyncReturn { ExactMatch, ParialMatch, NoMatch, Exception, BadParam }


        public SyncReturn Sync(Catalog catalog, String topicID, out TreeNode node)
        {
            node = null;
            if (catalog == null || !catalog.IsOpen || treeview == null || String.IsNullOrEmpty(topicID))
                return SyncReturn.BadParam;

            try
            {
                // returns topic[0] (level 0 root); topic[1] (level 1); ... topic[n] ( topicID )
                if (topicID != lastTopicID || ancestors == null)
                {
                    ancestors = (TopicCollection)catalogRead.GetTableOfContents(catalog, topicID, null, TocReturnDetail.TocAncestors);
                }
                lastTopicID = topicID;

                TreeNode lastMatch = null;

                //Fix: Beta release had item[0] = -1 but the release version no longer includes the artificial node. So we need to 

                if (ancestors.Count > 0 && topicID != "-1")
                {
                    ancestors.MoveTo(0);
                    Topic topic = (Topic)ancestors.Current;
                    if (topic.Id != "-1")
                    {
                        node = NodesContainId(treeview.Nodes, "-1");  //check "-1"
                    }
                }

                for (int i = 0; i < ancestors.Count; i++)
                {
                    ancestors.MoveTo(i);
                    Topic topic = (Topic)ancestors.Current;

                    // Find the ID (starting from root node) in the tree nodes

                    if (node == null)
                        node = NodesContainId(treeview.Nodes, topic.Id);  //root tree nodes
                    else
                    {
                        lastMatch = node;
                        node = NodesContainId(node.Nodes, topic.Id);
                    }

                    // Make some decisions

                    if (i == ancestors.Count - 1 && node != null)   //end of the array is topicId -- we an exact match
                    {
                        return SyncReturn.ExactMatch;
                    }
                    else if (node == null) // no match - return last good node match in the ancester match
                    {
                        node = lastMatch;
                        if (node == null)
                        {
                            return SyncReturn.NoMatch;    // this topic id is not in the master TOC
                        }
                        else
                        {
                            return SyncReturn.ParialMatch;   // We got matched just so far then ran out of nodes
                        }
                    }
                }

            }
            catch
            {
                return SyncReturn.Exception;
            }
            return SyncReturn.NoMatch;
        }

        private TreeNode NodesContainId(TreeNodeCollection nodes, String topicId)
        {
            foreach (TreeNode node in nodes)
            {
                if (node.Tag is Topic)
                {
                    Topic topic = (Topic)node.Tag;
                    if (topic.Id == topicId)
                        return node;           //return matching node with the ID
                }
            }

            return null;  // not found
        }


    }

}
