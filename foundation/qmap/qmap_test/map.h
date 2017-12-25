#ifndef MAP_H
#define MAP_H
#include <qmap.h>
#include <iostream>
#include <Qdebug>
using namespace std;
class MapTest
{
public:
    void showMap()
    {
        if(!m_map.isEmpty()) return; //判断map是否为空
        m_map["333"] = "ccc"; //另一种添加的方式
        if(!m_map.contains("222")) //判断map里是否已经包含某“键-值”
            m_map.insert("222", "bbb");
        m_map.insert("11", "bba"); //向map里添加一对“键-值”
        m_map.insert("11", "bbc"); //向map里添加一对“键-值”
        m_map.insert("511", "faa"); //向map里添加一对“键-值”
        m_map.insert("111", "aaa"); //向map里添加一对“键-值”
        qDebug() << "map[333] , value is : " << m_map["333"]; //这种方式既可以用于添加，也可以用于获取，但是你必须知道它确实存在

        if(m_map.contains("111")){
            QMap<QString,QString>::iterator it = m_map.find("111"); //找到特定的“键-值”对
            qDebug() << "find 111 , value is : " << it.value(); //获取map里对应的值
        }
        cout<< endl;
        qDebug("size of this map is : %d", m_map.count()); //获取map包含的总数
        cout<< endl;
        QMap<QString,QString>::iterator it; //遍历map
        for ( it = m_map.begin(); it != m_map.end(); ++it ) {
            qDebug() << QString("%1: %2").arg(it.key()).arg(it.value()); //用key()和data()分别获取“键”和“值”
        }

        m_map.clear(); //清空map
    }
private:
    QMap<QString,QString> m_map; //定义一个QMap对象
};

#endif // MAP_H
