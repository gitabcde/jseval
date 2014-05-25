// jstest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <jsapi.h>
#include <iostream>
#include <string>


int evaljs(std::string& jscode,std::string& retvalue)
{
    JSClass global_class = {
    "global", JSCLASS_GLOBAL_FLAGS,
    JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
    JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, JS_FinalizeStub,
    JSCLASS_NO_OPTIONAL_MEMBERS
    };

    JSRuntime* rt;
    JSContext* ctx;
    JSObject *global;
    jsval val;
    rt=JS_NewRuntime(8*1024*1024);
    if(!rt)
      return -1;
    ctx=JS_NewContext(rt,8192);
    if(ctx==NULL)
      return -1;
    global=JS_NewObject(ctx,&global_class,NULL,NULL);
    if(global==NULL)
      return -1;
    if(!JS_InitStandardClasses(ctx,global))
      return -1;
    JS_EvaluateScript(ctx,global,jscode.c_str(),jscode.size(),"",0,&val);
    JSString *str;
    str=JS_ValueToString(ctx,val);
    retvalue.append(JS_GetStringBytes(str));
    JS_DestroyContext(ctx);
    JS_DestroyRuntime(rt);
    JS_ShutDown();
    return 0;
}
void findjsfun(std::string& jscode,std::string& funname,std::string& funstr)
{
  std::string::size_type posbegin=0,posend=0;
    while((posbegin=jscode.find(funname,posbegin))!=std::string::npos)
    {
      if(jscode.substr(posbegin-9,8)=="function")
        break;
      posbegin+=funname.size();
    }
    posbegin-=9;
    int remaining=1; 
    posend=jscode.find("{",posbegin)+1;
    std::string::size_type posleft=posend,posright=posend;
    while(remaining!=0) 
    {
      posleft=jscode.find("{",posleft);
      posright=jscode.find("}",posright);
      if(posleft==std::string::npos)
      {
        if(posright!=std::string::npos)
        {
          posright++;
          posend=posright;
          remaining--;
          continue;
        }
        else
        {
          posend=std::string::npos;
          break;
        }
      }
      else
      {
        if(posright==std::string::npos) 
        {
          posend=std::string::npos;
          break;
        }
        else
        {
          if(posleft<posright)
          {
            remaining++;
            posleft++;
            posright=posleft;
            posend=posleft;
          }
          else
          {
            remaining--;
            posright++;
            posleft=posright;
            posend=posright;
          }
        }
      }
    }
    funstr.append(jscode.substr(posbegin,posend-posbegin));
    funstr.append(";");

}
void findinnerjsfun(std::string& funstr,std::string& innerfunname)
{
  std::string::size_type equalpos=0,bracketpos;
  while((equalpos=funstr.find("=",equalpos))!=std::string::npos)
  {
    equalpos++;
    bracketpos=funstr.find("(",equalpos);
    if(bracketpos==std::string::npos)
      break;
    bool legalfun=true;
    for(std::string::size_type begin=equalpos;begin<bracketpos;begin++)
    {
      if(!((funstr[begin]>='a' && funstr[begin]<='z') || (funstr[begin]>='A' && funstr[begin]<='Z') || (funstr[begin]>='0' && funstr[begin]<='9') || funstr[begin]=='_')) 
      {
        legalfun=false;
        break;
      }
    }
    if(legalfun)
    {
      innerfunname.append(funstr.substr(equalpos,bracketpos-equalpos)); 
      innerfunname.append(" ");
    }
 
  }
  
}


int _tmain(int argc, _TCHAR* argv[])
{
	std::string jscode="1+4";
	std::string ret;
	evaljs(jscode,ret);
	std::cout<<"ret is "<<ret<<std::endl;
}

