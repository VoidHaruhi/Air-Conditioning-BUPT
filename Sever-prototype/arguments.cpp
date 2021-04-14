#include "arguments.h"
#include<QDebug>
using namespace std;
map<QString,QString> parseArgument(string s){
    map<QString,QString> res;
    int flag=0;
    QString ss=QString::fromStdString(s);
    QStringList args=ss.split(' ');

    if(args[0]=="set-superuser"){
        int i=1;

        while(i<args.length()){
            if(args[i]=="--username"&&i+1<args.length()){
                res["username"]=args[i+1];
                i+=2;
            }else if(args[i]=="--password"&&i+1<args.length()){
                res["password"]=args[i+1];
                i+=2;
            }else if(args[i]=="--role"&&i+1<args.length()){
                res["role"]=args[i+1];
                i+=2;
            }else{
                qDebug()<<("Not a valid arg\n");
            }
        }
    }else{
        qDebug()<<("Not a valid arg\n");
    }
    if(flag!=3){
        qDebug()<<"Not valid: arg missing";
    }
    return res;
}
