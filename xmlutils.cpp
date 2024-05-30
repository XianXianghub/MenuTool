#include "xmlutils.h"



XmlUtils::XmlUtils(QString p)
{
   path = p;
}

XmlUtils::~XmlUtils()
{

}

void XmlUtils::WriteXml()
{
       //打开或创建文件
//       QFile file(path); //相对路径、绝对路径、资源路径都可以
//       if(!file.open(QFile::WriteOnly|QFile::Truncate)) //可以用QIODevice，Truncate表示清空原来的内容
//           return;

//       QDomDocument doc;
//       //写入xml头部
//       QDomProcessingInstruction instruction; //添加处理命令
//       instruction=doc.createProcessingInstruction("xml","version=\"1.0\" encoding=\"UTF-8\"");
//       doc.appendChild(instruction);

//       //根节点
//       QDomElement root=doc.createElement("config");
//       doc.appendChild(root);

//       QDomText text;
//       //添加元素listenpath
//       text=doc.createTextNode("D:/");
//       QDomElement listenpath=doc.createElement(LISTENPATH); //创建子元素
//       listenpath.appendChild(text);
//       root.appendChild(listenpath);


//       //添加元素cmdpath
//       text=doc.createTextNode("D:/");
//       QDomElement cmdpath=doc.createElement(CMDPATH); //创建子元素
//       cmdpath.appendChild(text);
//       root.appendChild(cmdpath);

//       //输出到文件
//       QTextStream out_stream(&file);
//       doc.save(out_stream,4); //缩进4格
//       file.close();
}

QString XmlUtils::ReadXml(QString key)
{
       if(map.isEmpty()){
           InitData();
       }
       return map.find(key).value();
}


void XmlUtils::UpdateXml(QString key, QString val)
{
      QFile file(path);
      if(!file.open(QFile::ReadOnly))
          return;
      QDomDocument doc;
      if(!doc.setContent(&file))
      {
          file.close();
          return;
      }
      file.close();

      QDomElement root=doc.documentElement(); //返回根节点
      qDebug()<<root.nodeName();
      QDomNode node=root.firstChild(); //获得第一个子节点
      while (!node.isNull())
      {
          QString tagName = node.toElement().tagName();  //节点元素名称
          if (tagName.compare(key) == 0) //节点标记查找
          {
              if(node.hasChildNodes())
              {
                  QDomNode oldnode = node.firstChild();
                  node.firstChild().setNodeValue(val);
                  QDomNode newnode = node.firstChild();
                  node.replaceChild(newnode,oldnode);
              }
              else
              {
                  QDomText text;
                  text = doc.createTextNode(val);
                  node.appendChild(text);
                  text.clear();
              }
              map.insert((key),val);
              break;
          }
          node = node.nextSibling();//读取下一个兄弟节点
      }


      if(!file.open(QFile::WriteOnly))
          return;
      //输出到文件
      QTextStream out_stream(&file);
      doc.save(out_stream,4); //缩进4格
      file.close();
}

QHash<QString, QString> XmlUtils::getConfigMap()
{
    return map;
}

void XmlUtils::InitData()
{
    QFile file(path); //相对路径、绝对路径、资源路径都行
    if(!file.open(QFile::ReadOnly))
        return ;

    QDomDocument doc;
    if(!doc.setContent(&file))
    {
        file.close();
         return ;
    }
    file.close();

    QDomElement root=doc.documentElement(); //返回根节点
//    qDebug()<<root.nodeName();
    QDomNode node=root.firstChild(); //获得第一个子节点
    while (!node.isNull())
    {
        QString tagName = node.toElement().tagName();  //节点元素名称
        map.insert(tagName,node.toElement().text());
        node = node.nextSibling();//读取下一个兄弟节点
    }
}

