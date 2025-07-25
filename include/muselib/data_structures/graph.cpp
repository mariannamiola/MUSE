#include "graph.h"

#include <iostream>
#include <iomanip>

// Function to print adjacency list representation of a graph
void printGraph(Graph const &graph, int n, std::deque<std::string> deps, std::deque<int> level)
{
    std::string root = "WORKFLOW";
    std::cout << root << std::endl;
    //std::cout << "|" << std::endl;

    std::string prefix = "├── ";
    std::string string;
    //for (int i = 0; i < n; i++)
    for (int i = n-1; i >= 0; i--)
    {
        int prev = i+1;

        if (i!=n-1 && graph.adjList[i].size() == 0 && level.at(i) == level.at(prev))
        {
            string = "|   " + deps.at(i);
        }
        else if (i==n-1 && graph.adjList[i].size() == 0)
        {
            string = prefix + deps.at(i);
        }
        else if (graph.adjList[i].size() == 0)
        {
            string = prefix + deps.at(i);
        }
        else
            string = deps.at(i) + " ——> ";

        std::cout << string ;

        // print the current vertex number
//        std::cout << deps.at(i) << " ——> ";
//        //cout << i << " ——> ";


        // print all neighboring vertices of a vertex `i`
        for (int v: graph.adjList[i])
        {
            std::cout << deps.at(v) << " ";
        }
        std::cout << std::endl;
        //std::cout << "|" << std::endl;
    }
    std::cout << "└── " << std::endl;
}

void printGraph2(Graph const &graph, int n, std::deque<std::string> deps, std::deque<int> level)
{
    std::string root = "WORKFLOW";
    std::cout << root << std::endl;
    std::string prefix = "├── ";
    //std::cout << prefix << std::endl;

    std::string string;
    for (int i = n-1; i >= 0; i--)
    {
        if(i != 0)
        {
            // print all neighboring vertices of a vertex `i`
            if(graph.adjList[i].size() > 0)
            {
                for (int v=0; v< graph.adjList[i].size(); v++)
                {
                    if(v >= 0 && graph.adjList[i].size() > 1 && v != graph.adjList[i].size()-1)
                    {
                        std::cout << prefix + deps.at(graph.adjList[i].at(v)) << " + ";
                    }
                    else if(v==0 || v == graph.adjList[i].size()-1)
                    {
                        std::cout << prefix + deps.at(graph.adjList[i].at(v));
                    }
                    else
                    {
                        if(graph.adjList[i].size() > 1)
                            std::cout << deps.at(graph.adjList[i].at(v)) << " + " ;
                        else
                            std::cout << deps.at(graph.adjList[i].at(v));
                    }
                }
                std::cout << " ——> " << deps.at(i) << std::endl;

            }
//            else
//                std::cout << prefix + " INPUT ";



//            if(graph.adjList[i].size() != 0)
//                std::cout << " ——> " << deps.at(i) << std::endl;
//            else
//                std::cout << " INPUT " <<  std::endl;
            //std::cout << std::endl;
        }
        else // se i==0 chiudo └──
        {
            // print all neighboring vertices of a vertex `i`
            for (int v: graph.adjList[i])
            {
                if(v == graph.adjList[i].size())
                    std::cout << "└── " << deps.at(v);
                else
                {
                    if(graph.adjList[i].size() > 1)
                        std::cout << "└── " << deps.at(v) << " + " ;
                    else
                        std::cout << "└── " << deps.at(v);
                }
            }
            if(graph.adjList[i].size() != 0)
                std::cout << " ——> " << deps.at(i) << std::endl;
            std::cout << std::endl;
        }
    }

}



void printGraph2(Graph const &graph, int n, std::deque<std::string> &deps)
{
    std::string root = "WORKFLOW";
    std::cout << root << std::endl;
    //std::cout << prefix << std::endl;

    for (int i = n-1; i >= 0; i--)
    {
        std::string prefix = (i != 0) ? "├── " : "└── ";
        std::string string = prefix;

        // print all neighboring vertices of a vertex `i`
        if(graph.adjList[i].size() > 0)
        {
            for (size_t v=0; v< graph.adjList[i].size(); v++)
            {
                if(graph.adjList[i].size() == 1)
                    string += deps.at(graph.adjList[i].at(v));
                else
                {
                    if(v == graph.adjList[i].size()-1)
                        string += deps.at(graph.adjList[i].at(v));
                    else
                        string += deps.at(graph.adjList[i].at(v)) + " + ";
                }
            }

            if(string.find("in/") != std::string::npos)
                std::cout << "\e[1m" << string << "\e[0m" << " ——> " << deps.at(i) << std::endl;
            else
                std::cout << string << " ——> " << deps.at(i) << std::endl;
        }
    }
}

void printGraph2(Graph const &graph, int n, std::deque<std::string> &deps, std::deque<std::string> &com)
{
    std::string root = "WORKFLOW";
    std::cout << root << std::endl;
    //std::cout << prefix << std::endl;

    for (int i = n-1; i >= 0; i--)
    {
        std::string prefix = (i != 0) ? "├── " : "└── ";
        std::string string = prefix;

        // print all neighboring vertices of a vertex `i`
        if(graph.adjList[i].size() > 0)
        {
            for (size_t v=0; v< graph.adjList[i].size(); v++)
            {
                if(graph.adjList[i].size() == 1)
                    string += deps.at(graph.adjList[i].at(v));
                else
                {
                    if(v == graph.adjList[i].size()-1)
                        string += deps.at(graph.adjList[i].at(v));
                    else
                        string += deps.at(graph.adjList[i].at(v)) + " + ";
                }
            }

            if(string.find("in/") != std::string::npos)
                std::cout << "\e[1m" << string << "\e[0m" << " ——> " << deps.at(i) << std::endl;
            else
                std::cout << string << " ——> " << deps.at(i) << std::endl;

            std::cout << std::setfill(' ') << std::right << std::setw(string.length()+3) << " ——> " << com.at(i) << std::endl;
        }
    }
}


void printGraph2_old(Graph const &graph, int n, std::deque<std::string> &deps)
{
    std::string root = "WORKFLOW";
    std::cout << root << std::endl;
    std::string prefix;
    //std::cout << prefix << std::endl;

    for (int i = n-1; i >= 0; i--)
    {
        std::string string;
        if(i != 0)
        {
            prefix = "├── ";

            // print all neighboring vertices of a vertex `i`
            if(graph.adjList[i].size() > 0)
            {
                for (size_t v=0; v< graph.adjList[i].size(); v++)
                {
                    if(v >= 0 && graph.adjList[i].size() > 1) // && v != graph.adjList[i].size()-1)
                    {
                        if(v == graph.adjList[i].size()-1)
                            string += prefix + deps.at(graph.adjList[i].at(v));
                        else
                            string += prefix + deps.at(graph.adjList[i].at(v)) + " + ";
                        //std::cout << prefix + deps.at(graph.adjList[i].at(v)) << " + ";
                    }
                    else if(v==0 || v == graph.adjList[i].size()-1)
                    {
                        //string = deps.at(graph.adjList[i].at(v));
                        string = prefix + deps.at(graph.adjList[i].at(v));
                        //std::cout << prefix + deps.at(graph.adjList[i].at(v));
                    }
                    else
                    {
                        if(graph.adjList[i].size() > 1)
                            string += deps.at(graph.adjList[i].at(v)) + " + " ;
                        //std::cout << deps.at(graph.adjList[i].at(v)) << " + " ;
                        else
                            string = deps.at(graph.adjList[i].at(v));
                        //std::cout << deps.at(graph.adjList[i].at(v));
                    }
                }
                if(string.find("in/") != std::string::npos)
                    std::cout << "\e[1m" << string << "\e[0m" << " ——> " << deps.at(i) << std::endl;
                else
                    std::cout << string << " ——> " << deps.at(i) << std::endl;
            }
        }
        else // se i==0 chiudo └──
        {
            prefix = "└── ";

            if(graph.adjList[i].size() > 0)
            {
                // print all neighboring vertices of a vertex `i`
                for (size_t v: graph.adjList[i])
                {
                    if(v == graph.adjList[i].size())
                        string = prefix + deps.at(v);
                    //std::cout << "└── " << deps.at(v);
                    else
                    {
                        if(graph.adjList[i].size() > 1)
                            string = prefix + deps.at(v) + " + ";
                        //std::cout << "└── " << deps.at(v) << " + " ;
                        else
                            string = prefix + deps.at(v);
                        //std::cout << "└── " << deps.at(v);
                    }
                }
                if(string.find("in/") != std::string::npos)
                    std::cout << "\e[1m" << string << "\e[0m" << " ——> " << deps.at(i) << std::endl;
                else
                    std::cout << string << " ——> " << deps.at(i) << std::endl;
            }

            std::cout << std::endl;
        }
    }

}


void printGraph2_old(Graph const &graph, int n, std::deque<std::string> &deps, std::deque<std::string> &com)
{
    std::string root = "WORKFLOW";
    std::cout << root << std::endl;
    std::string prefix;
    //std::cout << prefix << std::endl;

    for (int i = n-1; i >= 0; i--)
    {
        std::string string;
        if(i != 0)
        {
            prefix = "├── ";

            // print all neighboring vertices of a vertex `i`
            if(graph.adjList[i].size() > 0)
            {
                for (size_t v=0; v< graph.adjList[i].size(); v++)
                {
                    if(v >= 0 && graph.adjList[i].size() > 1 && v != graph.adjList[i].size()-1)
                    {
                        string = prefix + deps.at(graph.adjList[i].at(v)) + " + ";
                        //std::cout << prefix + deps.at(graph.adjList[i].at(v)) << " + ";
                    }
                    else if(v==0 || v == graph.adjList[i].size()-1)
                    {
                        string = prefix + deps.at(graph.adjList[i].at(v));
                        //std::cout << prefix + deps.at(graph.adjList[i].at(v));
                    }
                    else
                    {
                        if(graph.adjList[i].size() > 1)
                            string = deps.at(graph.adjList[i].at(v)) + " + " ;
                            //std::cout << deps.at(graph.adjList[i].at(v)) << " + " ;
                        else
                            string = deps.at(graph.adjList[i].at(v));
                            //std::cout << deps.at(graph.adjList[i].at(v));
                    }
                }
                if(string.find("in/") != std::string::npos)
                    std::cout << "\e[1m" << string << "\e[0m" << " ——> " << deps.at(i) << std::endl;
                else
                    std::cout << string << " ——> " << deps.at(i) << std::endl;
                std::cout << std::setfill(' ') << std::right << std::setw(string.length()+3) << " ——> " << com.at(i) << std::endl;
            }
        }
        else // se i==0 chiudo └──
        {
            prefix = "└── ";

            if(graph.adjList[i].size() > 0)
            {
                // print all neighboring vertices of a vertex `i`
                for (size_t v: graph.adjList[i])
                {
                    if(v == graph.adjList[i].size())
                        string = prefix + deps.at(v);
                        //std::cout << "└── " << deps.at(v);
                    else
                    {
                        if(graph.adjList[i].size() > 1)
                            string = prefix + deps.at(v) + " + ";
                            //std::cout << "└── " << deps.at(v) << " + " ;
                        else
                            string = prefix + deps.at(v);
                            //std::cout << "└── " << deps.at(v);
                    }
                }
                if(string.find("in/") != std::string::npos)
                    std::cout << "\e[1m" << string << "\e[0m" << " ——> " << deps.at(i) << std::endl;
                else
                    std::cout << string << " ——> " << deps.at(i) << std::endl;
                std::cout << std::setfill(' ') << std::right << std::setw(string.length()+3) << " ——> " << com.at(i) << std::endl;
            }

            std::cout << std::endl;
        }
    }

}


//class Node {
// public:
//    Node(std::string val) {
//        this->val = val;
//    }
//    std::vector<Node*> _prev;
//    std::vector<Node*> _children;
//    std::string val;

//void printSubtree(const std::string &prefix)
//{
//    using std::cout;
//    using std::endl;

//    if (_children.empty())
//        return;
//    cout << prefix;

//    size_t n_children = _children.size();
//    cout << (n_children > 1 ? "├── " : "");

//    for (size_t i = 0; i < n_children; ++i) {
//        Node *c = _children[i];
//        if (i < n_children - 1) {
//            if (i > 0) { // added fix
//                cout << prefix<< "├── "; // added fix
//            } // added fix
//            bool printStrand = n_children > 1 && !c->_children.empty();
//            std::string newPrefix = prefix + (printStrand ? "│\t" : "\t");
//            std::cout << c->val << "\n";
//            c->printSubtree(newPrefix);
//        } else {
//            cout << (n_children > 1 ? prefix : "") << "└── ";
//            std::cout << c->val << "\n";
//            c->printSubtree(prefix + "\t");
//        }
//    }
//}

//    void printTree() {
//        using std::cout;
//        std::cout << val << "\n";
//        printSubtree("");
//        cout << "\n";
//    }
//};
