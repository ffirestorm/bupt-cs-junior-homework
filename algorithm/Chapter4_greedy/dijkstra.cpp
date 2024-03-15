#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include <unordered_map>
#include <limits>
#include <fstream>
#include <string>
#include <sstream>
#include <math.h>
#include <stack>
#define MAXINT 1000000000
using namespace std;
// 将基站id转为数组的索引
int getIndexFromId(int id, vector<int>ids){
    for(int i = 0; i < ids.size();i ++){
        if(id == ids[i])
            return i;
    }
    return -1;
}
// 获取用户输入的终点
int getInputEnd(vector<int>ids){
    int id;
    for(int i = 0; i < ids.size();i++){
        cout << ids[i] << "\t";
    }
    cout << endl <<"请从以上基站ID中选择一个当做终点:";
    cin >> id;
    while(true){
        auto it = find(ids.begin(), ids.end(), id);
        if(it == ids.end()){
            cout << "输入不合法，请重新输入"<< endl;
            cin >> id;
        }else{
            break;
        }
    }
    return getIndexFromId(id, ids); 
}
// 获取用户输入的起点
int getInputStart(vector<int>ids){
    int id;
    for(int i = 0; i < ids.size();i++){
        cout << ids[i] << "\t";
    }
    cout << endl <<"请从以上基站ID中选择一个当做起点:" ;
    cin >> id;
        while(true){
        auto it = find(ids.begin(), ids.end(), id);
        if(it == ids.end()){
            cout << "输入不合法，请重新输入"<< endl;
            cin >> id;
        }else{
            break;
        }
    }
    return getIndexFromId(id, ids); 
}

int getIdFromIndex(int index, vector<int>ids){
    return ids[index];
}
// 完整的路径
vector<int> getPath(vector<int>path, int v, int end)
{
    vector<int> p;
    p.push_back(end);
    while(end != v){
        end = path[end];
        p.push_back(end);
    }
    reverse(p.begin(), p.end());
    return p;
}

vector<int> getPrePath(vector<vector<double>> graph, int v){
    // 初始化 s 集合，只有一个起始点在里面
    int n = graph[0].size();
    vector<bool> s(n, false);
    vector<double> dist(n, MAXINT);
    vector<int> path(n, -1);    
    s[v] = true;
    dist[v] = 0;
    // 进行最短路径算法
    for(int i = 0; i < n; i++){
        // 考虑原点v之外的其他点u，对于不同规模si，逐步扩展
        int temp = MAXINT;
        int u = v;
        for(int j = 0; j < n; j++){
            //选取s外具有最小dist的点
            if(!s[j] && dist[j] < temp){
                u = j;
                temp = dist[j];
            }
        }
        s[u] = true;
        for(int j = 0; j < n; j++){
            if(!s[j] && graph[u][j] != MAXINT 
            && dist[j] > graph[u][j]+dist[u]){
                dist[j] = graph[u][j] + dist[u];    
                path[j] = u;
            }
        }
    }
    return path;
}

vector<double> dijkstra(vector<vector<double>> graph, int v){
    // 初始化 s 集合，只有一个起始点在里面
    int n = graph[0].size();
    vector<bool> s(n, false);
    vector<double> dist(n, MAXINT);    
    s[v] = true;
    dist[v] = 0;
    // 进行最短路径算法
    for(int i = 0; i < n; i++){
        // 考虑原点v之外的其他点u，对于不同规模Si，逐步扩展
        int temp = MAXINT;
        int u = v;
        for(int j = 0; j < n; j++){
            //选取s外具有最小dist的点
            if(!s[j] && dist[j] < temp){
                u = j;
                temp = dist[j];
            }
        }
        s[u] = true;
        for(int j = 0; j < n; j++){
            if(!s[j] && graph[u][j] != MAXINT 
            && dist[j] > graph[u][j]+dist[u]){
                dist[j] = graph[u][j] + dist[u];    
            }
        }
    }
    return dist;
}

// 用曼哈顿距离作为启发函数
double heuristic(vector<vector<double>> &graph, int u, int goal)
{
    int n = graph.size();
    int uRow = u / n;
    int uCol = u % n;
    int goalRow = goal / n;
    int goalCol = goal % n;

    return abs(uRow - goalRow) + abs(uCol - goalCol);
}

vector<int> reconstructPath(unordered_map<int, int> &cameFrom, int u)
{
    vector<int> totalPath = {u};
    while (cameFrom.find(u) != cameFrom.end())
    {
        u = cameFrom[u];
        totalPath.push_back(u);
    }
    reverse(totalPath.begin(), totalPath.end());
    return totalPath;
}

vector<int> AStar(vector<vector<double>> &graph, int start, int goal)
{
    int n = graph.size();

    set<int> openList;
    vector<bool> s(graph[0].size(), false);
    // g(n)代表从起始点到当前点n的开销
    unordered_map<int, double> dist;

    // h(n)代表当前点n到终点goal的估算开销
    unordered_map<int, double> h;

    // f(n) = g(n) + h(n)
    unordered_map<int, double> f;

    unordered_map<int, int> cameFrom;

    dist[start] = 0;
    h[start] = heuristic(graph, start, goal);
    f[start] = dist[start] + h[start];
    // 1. 先将起始点放入开放列表
    openList.insert(start);
    while (!openList.empty())
    {
        // 优先队列，优先级设置为f(n)
        int u = *min_element(openList.begin(), openList.end(),
            [&](int a, int b){ return f[a] < f[b]; });

        if (u == goal)
        {
            return reconstructPath(cameFrom, goal);
        }
        openList.erase(u);
        s[u] = true;

        for (int j = 0; j < n; ++j)
        {
            // 查找不在关闭队列中的
            if (graph[u][j] != MAXINT && !s[j])
            {
                double tempDist = dist[u] + graph[u][j];
                if (openList.find(j) == openList.end() || tempDist < dist[j])
                {
                    // 更新数据
                    cameFrom[j] = u;
                    dist[j] = tempDist;
                    h[j] = heuristic(graph, j, goal);
                    f[j] = dist[j] + h[j];
                    openList.insert(j);
                }
            }
        }
    }

    // 如果openList空了还没找到终点，就返回空
    return vector<int>();
}


int main(){
    vector<vector<double>> graph;
    ifstream inputFile("./1-1-2.txt");
    if(!inputFile.is_open()){
        cerr << "文件打开失败" << endl;
        return 1;
    }
    string line;
    getline(inputFile, line);
    stringstream ss(line);
    // 读取第一行基站数据
    int id;
    double value;
    vector<int> ids;
    while(ss >> id){
        ids.push_back(id);
    }
    // 跳过第一列读取距离数据
    while(inputFile >> id){
        vector<double> row;
        for(int i = 0; i < ids.size(); i++){
            inputFile >> value;
            if(value+1 < abs(1e-4))
                value = MAXINT;
            row.push_back(value);
        }
        graph.push_back(row);
    }
    inputFile.close();

    // 用户输入起始位置
    int start = getInputStart(ids); 
    vector<double>dist = dijkstra(graph, start);
    // int end = getInputEnd(ids);
    for(int i = 0 ; i < ids.size(); i++){
        int end = i;
        vector<int>prePath = getPrePath(graph, start);
        vector<int>path = getPath(prePath, start, end);
        cout << "=================以下是普通算法迪杰斯特拉的结果=================="<< endl;
        cout << "dijkstra到达终点的路径为:" ;
        for(int i = 0 ; i < path.size()-1; i++){
            cout << getIdFromIndex(path[i], ids) << " --> ";
        }
        cout << getIdFromIndex(end, ids) << endl << "最短的距离是: " << dist[end] <<endl;

        cout << "=================以下是改进算法A* 的结果=================="<< endl;
        vector<int>aPath = AStar(graph, start, end);
        double d = 0;
        // 获取路径
        cout << " A*从起点到终点的路径为: " ;
        for (int i = 1; i < aPath.size(); i++){
            d += graph[aPath[i - 1]][aPath[i]];
        }
        for (int i = 0; i < aPath.size(); i++){
            if(i != aPath.size()-1)
                cout << getIdFromIndex(aPath[i], ids) << " --> ";
            else
                cout << getIdFromIndex(aPath[i], ids) << endl;
        }
        cout << "最短距离是:" << d << endl;
    }
    
    return 0;
}