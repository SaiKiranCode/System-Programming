#include<iostream>
#include<fstream>
#include<map>
#include<set>
#include<stdio.h>
#include<vector>
#include<stdlib.h>
#include<sstream>
#include<ctype.h>
#include<iomanip>
#include<algorithm>
using namespace std;
map<string, pair<int,string>> mymap;
map<string,string> symtab,register_map,binary_map,reverse_binary_map;
string pg_size,base_argument,loc;
string format3,format4,tab_string="    ",T;
bool er=false;
const vector<string> explode(const string& s, const char& c)
{
	string buff{""};
	vector<string> v;

	for(auto n:s)
	{
		if(n != c) buff+=n; else
		if(n == c && buff != "") { v.push_back(buff); buff = ""; }
	}
	if(buff != "") v.push_back(buff);

	return v;
}

void make_opmap()
{
    FILE *fp;
	fp = fopen("optable.txt","r");
	char instruction[30];
	int format,ch;		char opc[30];
	while(1){
		fscanf(fp,"%s",instruction);
		fscanf(fp,"%d", &format);
		fscanf(fp,"%s", opc);
		mymap[instruction]=make_pair(format,opc);
		if((ch=fgetc(fp))==EOF)
			break;
	}


}

string convert_loc(string label,int loc_decimal,int siz=0)
{
    stringstream ss;
    loc_decimal+=siz;
    ss<< std::hex << loc_decimal;
    std::string res ( ss.str() );
    switch(res.size())
    {
     case 1:
         res="000"+res;break;
     case 2:
         res="00"+res;break;
     case 3:
         res="0"+res;break;
    }
    if(label.size()>0)
        symtab[label]=res;//symtab used here
    res="0x"+res;
    return res;
}

char* str_convert(string s)
{
    char *ch=new char[s.size()+1];
    int i;
    for(i=0;i<s.size();i++)
    {
        ch[i]=s[i];

    }
    ch[i]='\0';

    return ch;
}

string algorithm_pass1(string fn)
{
    bool error_flag=false;
    ifstream fp(fn);//change
    ofstream fp2("intermediate.txt");

    string label,inst,arguments,loc;
    int ch,c,format,word=0; string s;
    while(getline(fp,s))
    {
        if(s.size()>0)
        {
            vector<string> vi{explode(s, ' ')};
        if(mymap.count(vi[0])>0)
        {
            inst=vi[0];
            arguments=vi[1];
        }
        else
        {
            label=vi[0];
            inst=vi[1];
            arguments=vi[2];
        }


        if(inst=="START")
        {
            if(mymap.count(label)==0)
            {
                loc="0x0000";
                symtab[label]="0000";// symtab used here
                string str="0000";
                str+=tab_string+s;
                fp2<<str<<endl;
            }
            else
            {
                cout<<"Invalid label\n";
                error_flag=true;
            }

        }
        else
        {

            loc="0x0000";
            int loc_decimal;
            if(mymap.count(label)==0)
            {
                while(1)
                {

                    if(s.size()>0)
                    {
                        char *ch=str_convert(loc);
                        sscanf(ch,"%x",&loc_decimal);

                        vector<string> v{explode(s, ' ')};
                        if(v.size()==2)
                        {
                            inst=v[0];
                            arguments=v[1];
                        }
                        else if(v.size()==3)
                        {
                            label=v[0];
                            inst=v[1];
                            arguments=v[2];
                        }
                        else
                        {
                            inst=v[0];
                        }

                        if(mymap.count(label)==0)
                        {
                            if(inst=="END")
                            {

                                string str2=s;
                                fp2<<str2<<endl;
                                pg_size=loc;
                                break;
                            }
                            if(inst!="EQU")
                            {
                                string str2;
                                if(inst=="BASE")
                                    str2=tab_string+tab_string+s;
                                else str2=loc.substr(2)+tab_string+s;
                                fp2<<str2<<endl;
                            }

                           
                            //Handling Symbols



                            if(label.size()>0)
                            {
                               if(symtab.count(label)>0)
                               {
                                   cout<<"Multiple symbol\n";
                                    error_flag=true;
                                    break;
                               }
                               else
                               {
                                   loc=convert_loc(label,loc_decimal,0);

                               }

                            }
                               //Handling opcodes
                           if(mymap.count(inst)>0||mymap.count(inst.substr(1)))
                           {

                               if(inst[0]=='+')
                               {

                                   loc=convert_loc("",loc_decimal,4);
                               }
                               else
                               {
                                   std::map<string,pair<int,string>>::iterator it;
                                   it=mymap.find(inst);
                                   if(it->second.first==1)
                                       loc=convert_loc("",loc_decimal,1);
                                   else if(it->second.first==2)
                                       loc=convert_loc("",loc_decimal,2);
                                   else if(it->second.first==3)
                                       loc=convert_loc("",loc_decimal,3);
                               }
                           }
                           else if(inst=="WORD")
                           {
                               loc=convert_loc("",loc_decimal,3);
                           }
                           else if(inst=="RESB")
                           {
                               int siz;
                               siz=atoi(arguments.c_str());
                               loc=convert_loc("",loc_decimal,siz);
                           }
                           else if(inst=="RESW")
                           {
                               int siz=atoi(arguments.c_str());
                               loc=convert_loc("",loc_decimal,siz*3);
                           }
                           else if(inst=="BYTE") 
                           {
                               if(arguments[0]=='C')
                                    loc=convert_loc("",loc_decimal,3);
                               else loc=convert_loc("",loc_decimal,1);

                           }
                           else if(inst=="EQU")
                           {
                               if(arguments=="*")
                               {
                                   string str=loc.substr(2)+tab_string+s+"\n";
                                   fp2<<str;
                                   symtab[label]=loc.substr(2);
                               }
                               else
                               {
                                   vector<string> v_equ{explode(arguments, '-')};
                                   int x=atoi(symtab[v_equ[0]].c_str()),y=atoi(symtab[v_equ[1]].c_str());
                                   int z=x-y;
                                   stringstream ss;
                                   ss<<z;
                                   string res(ss.str());
                                   switch(res.size())
                                   {
                                   case 1:
                                        res="000"+res;break;
                                   case 2:
                                        res="00"+res;break;
                                   case 3:
                                        res="0"+res;break;
                                   }
                                   res=res+tab_string+s+"\n";
                                   fp2<<res;

                               }
                           }
                           else if(inst=="BASE")
                           {
                               base_argument=arguments;

                           }
                           else
                           {
                               cout<<"No inst\n";
                               error_flag=true;
                               break;
                           }

                           label="";
                           inst="";
                           arguments="";
                    }
                    else
                    {
                        cout<<"Invalid label\n";
                        error_flag=true;
                        break;

                    }

                    }

                   getline(fp,s);

                }
            }
            else
            {
                cout<<"Invalid label\n";
                error_flag=true;
            }
            if(error_flag)
            {
                cout<<"Aborting!!!\n";
                break;
            }
        }
        label="";
        }

    }
    symtab["BASE"]=symtab[base_argument];
    fp.close();
    fp2.close();
    er=error_flag;
    return loc;
}



string OBJ_CALC(string PC,string EA)
{
    char *p=str_convert(PC),*e=str_convert(EA);
    int pc_dec,ea_dec;
    sscanf(p,"%x",&pc_dec);
    stringstream ss;
    sscanf(e,"%x",&ea_dec);
    int x=ea_dec-pc_dec;
    ss<< std::hex << x;
    std::string res ( ss.str() );

    if(res.size()<4)
    {
        switch(res.size())
        {
        case 1:
            return "00000000"+binary_map[res];
        case 2:
            return "0000"+binary_map[res.substr(0,1)]+binary_map[res.substr(1,1)];
        case 3:return binary_map[res.substr(0,1)]+binary_map[res.substr(1,1)]+binary_map[res.substr(2,1)];
        }
    }
    else
    {
        if(res[4]=='f')
        {
            char *p=str_convert("0x"+res.substr(5));
            int dec;
            sscanf(p,"%x",&dec);
            stringstream ss1;
            ss1<< std::hex << dec;
            std::string res2 ( ss1.str() );
            res2=binary_map[res2.substr(0,1)]+binary_map[res2.substr(1,1)]+binary_map[res2.substr(2,1)];
            return res2;
        }
        else
        {
            string base=symtab["BASE"];
            char *b=str_convert(base);
            int b_dec;
            sscanf(b,"%x",&b_dec);
            int x=ea_dec-b_dec;
            stringstream ss1;
            ss1<< std::hex << x;
            std::string res2 ( ss1.str() );
            format3[9]='1';format3[10]='0';
            switch(res2.size())
            {
                case 1:
                    return "00000000"+binary_map[res2];
                case 2:
                    return "0000"+binary_map[res2.substr(0,1)]+binary_map[res2.substr(1,1)];
                case 3:return binary_map[res2.substr(0,1)]+binary_map[res2.substr(1,1)]+binary_map[res2.substr(2,1)];
            }
        }
    }
}


void algorithm_pass2()
{
    bool error_flag=false;
    ifstream fp("intermediate.txt"),fp1("intermediate.txt");
    ofstream fp2("pass2_output.txt");

    string label,inst,arguments,opr_addr,loc2;
    int ch,c,format,word=0; string s,s1;
    getline(fp1,s1);
    while(getline(fp,s))
    {

        getline(fp1,s1);
        vector<string> vi{explode(s, ' ')};
        loc2=vi[0];
        label=vi[1];
        inst=vi[2];
        arguments=vi[3];
        if(inst=="START")
        {
            loc2="0x0000";
            symtab[label]="0000";
            string str=s;
            fp2<<str<<endl;

        }
        else
        {
            while(1)
            {

                vector<string> v2{explode(s1, ' ')};
                if(v2[0]=="BASE")
                {
                    getline(fp1,s1);

                    v2=explode(s1,' ');
                }

                vector<string> v{explode(s, ' ')};
                if(v[0]=="BASE")
                {
                    getline(fp,s);
                    v=explode(s,' ');
                }
                if(v.size()==4)
                {
                    loc2=v[0];
                    label=v[1];
                    inst=v[2];
                    arguments=v[3];
                }
                else if(v.size()==3)
                {
                    loc2=v[0];
                    label="";
                    inst=v[1];
                    arguments=v[2];
                }
                else if(v.size()==2)
                {
                    loc2=v[0];
                    inst=v[1];
                    arguments="";
                }


                //Searching for opcode in optable

                if(mymap.count(inst)>0||mymap.count(inst.substr(1))>0)
                {
                    if(arguments.size()>0&&inst[inst.size()-1]!='R')
                    {
                        if(arguments[0]=='#' && symtab.count(arguments.substr(1))==0)
                            opr_addr=arguments.substr(1);
                        else if(symtab.count(arguments.substr(1))>0)
                            opr_addr=symtab[arguments.substr(1)];
                        else if(symtab.count(arguments)>0)
                        {
                            opr_addr=symtab[arguments];
                        }
                        else
                        {
                            size_t found=arguments.find(",");
                            if(found==string::npos)
                            {
                                cout<<"Symbol Not Found::\n";
                                error_flag=true;
                            }

                        }



                    }
                    else
                    {
                        opr_addr="0";
                    }
                    //Checking fomats


                    if(inst[inst.size()-1]=='R')
                    {
                        //Format 2
                        pair<int,string> p=mymap[inst];
                        string obj_code=p.second;
                        if(arguments.size()==1)
                        {
                            obj_code+=register_map[arguments]+"0";
                            string str=s+tab_string+obj_code;
                            fp2<<str<<endl;
                        }
                        else
                        {
                            obj_code+=register_map[arguments.substr(0,1)]+register_map[arguments.substr(2,1)];
                            string str=s+tab_string+obj_code;
                            fp2<<str<<endl;
                        }
                    }
                    else if(inst[0]!='+')
                    {
                        //Format 3
                        pair<int,string> p=mymap[inst];
                        format3="";
                        string op_code=p.second;

                        string s3=binary_map[op_code.substr(1,1)];
                        format3=binary_map[op_code.substr(0,1)]+s3.substr(0,2);
                        if(arguments.size()>0)
                        {
                            if(arguments[0]=='#')
                                format3+="01";
                            else if(arguments[0]=='@')
                                format3+="10";
                            else
                                format3+="11";
                            //Checking if indexed
                            size_t found=arguments.find(",");
                            if(found==string::npos)
                                format3+="0";
                            else
                            {
                                format3+="1";
                                arguments=arguments.substr(0,found);
                            }

                            //Assuming it is PC relative
                            if(arguments[0]=='#'&&(arguments[1]>=48 && arguments[1]<=57))
                                format3+="000";
                            else format3+="010";
                            //Calculating displacement
                            if(arguments[0]=='#'&&(arguments[1]>=48 && arguments[1]<=57))
                            {
                                string s=arguments.substr(1);
                                int x=atoi(s.c_str());
                               
                                stringstream ss;
                                ss<< std::hex << x;
                                std::string res ( ss.str() );
                                switch(res.size())
                                {
                                    case 1:res="00"+res;break;
                                    case 2:res="0"+res;break;
                                }
                                format3+=binary_map[res.substr(0,1)]+binary_map[res.substr(1,1)]+binary_map[res.substr(2,1)];

                            }
                            else
                            {
                                string PC=v2[0],EA;
                                if(arguments[0]=='#'||arguments[0]=='@')
                                    EA=symtab[arguments.substr(1)];
                                else EA=symtab[arguments];
                                format3+=OBJ_CALC(PC,EA);

                            }
                            string st=reverse_binary_map[format3.substr(0,4)]+reverse_binary_map[format3.substr(4,4)]+reverse_binary_map[format3.substr(8,4)]+reverse_binary_map[format3.substr(12,4)]+reverse_binary_map[format3.substr(16,4)]+reverse_binary_map[format3.substr(20,4)];
                            string str=s+tab_string+st;
                            fp2<<str<<endl;
                        }
                        else
                        {
                            format3=op_code.substr(0,1)+reverse_binary_map[binary_map[op_code.substr(1,1)].substr(0,2)+"11"]+"0000";
                            fp2<<s+tab_string+format3<<endl;
                        }


                    }
                    else if(inst[0]=='+')
                    {
                        //format 4
                        pair<int,string> p=mymap[inst.substr(1)];
                        string st=p.second,temp;
                        if(arguments[0]=='#')
                        {
                            format4=st.substr(0,1)+reverse_binary_map[binary_map[st.substr(1,1)].substr(0,2)+"01"];
                            if((arguments[1]>=48 && arguments[1]<=57))
                            {
                                    format4+="1";
                                    int x=atoi(arguments.substr(1).c_str());
                                    stringstream ss;
                                    ss<< std::hex << x;
                                    std::string res ( ss.str() );
                                    switch(res.size())
                                    {
                                        case 1:res="0000"+res;break;
                                        case 2:res="000"+res;break;
                                        case 3:res="00"+res;break;
                                        case 4:res="0"+res;break;

                                    }

                                    format4+=res;
                            }
                            else
                            {
                                format4+="0"+symtab[arguments.substr(1)];
                            }
                        }
                        else
                        {
                            format4=st.substr(0,1)+reverse_binary_map[binary_map[st.substr(1,1)].substr(0,2)+"11"];
                            size_t found=arguments.find(",");
                            if(found==string::npos)
                            {
                                temp="0";
                                temp+="001";
                                format4+=reverse_binary_map[temp]+"0"+symtab[arguments];
                            }

                            else
                            {
                                temp="1";
                                temp+="001";

                                format4+=reverse_binary_map[temp]+"0"+symtab[arguments.substr(0,found)];
                            }

                        }
                        string str=s+tab_string+format4;
                        fp2<<str<<endl;

                    }
                    else
                    {
                        error_flag=true;
                        cout<<"No format found\n";

                    }

                }
                else if(inst=="BYTE")
                {
                    //Format 1
                    string obj_code;
                    if(arguments[0]=='X')
                    {
                        obj_code=arguments.substr(2,2);
                        string str=s+tab_string+obj_code;
                        fp2<<str<<endl;
                    }
                    else if(arguments[0]=='C')
                    {
                        obj_code="454F46";
                        string str=s+tab_string+obj_code;
                        fp2<<str<<endl;
                    }

                }
                else if(inst=="WORD")
                {
                    string obj_code=arguments;
                    stringstream ss;
                    ss<< std::hex << obj_code;
                    std::string res ( ss.str() );
                    switch(res.size())
                    {
                        case 1:res="00000"+res;break;
                        case 2:res="0000"+res;break;
                        case 3:res="000"+res;break;
                        case 4:res="00"+res;break;
                        case 5:res="0"+res;break;

                    }
                    res=s+tab_string+res;
                    fp2<<res<<endl;
                }
                else if(inst=="RESW"||inst=="RESB"||inst=="EQU")
                    fp2<<s<<endl;
                else
                {
                    cout<<"Instruction not found\n";
                    error_flag=true;

                }

                if(error_flag)
                {
                    cout<<"Aborting error!!\n"<<endl;
                    break;
                }
                if(v2[0]=="END")
                {
                    fp2<<s1<<endl;
                    error_flag=true;
                    break;
                }
                getline(fp,s);
                getline(fp1,s1);

            }



        }
      if(error_flag)
        break;

    }
    fp.close();
    fp1.close();
    fp2.close();
}


void Text_Record_Size(int counter)
{
    stringstream ss;
    ss<<hex<<counter;
    string res(ss.str());
    if(res.size()==1)
    {
        T[9]='0';
        T[10]=res[0];
    }
    else
    {
        T[9]=res[0];T[10]=res[1];
    }

}

void write_object_code()
{
    set<string> mod_record;
    bool flag=false;
    ifstream fp("pass2_output.txt");
    ofstream fp2("object_code.txt");
    string s,inst,args,obj;
    int counter=0;
    while(getline(fp,s))
    {
        obj="";
        inst="";

        vector<string> vi{explode(s, ' ')};
        if(vi.size()==2)
        {
            Text_Record_Size(counter);
            T=T.substr(0,T.size()-1);
            transform(T.begin(), T.end(),T.begin(), ::toupper);
            fp2<<T<<endl;
            for(auto it=mod_record.begin();it!=mod_record.end();it++)
            {
                char *p=str_convert((string)*it);
                int dec;
                sscanf(p,"%x",&dec);
                dec+=1;
                stringstream ss;
                ss<<hex<<dec;
                string res(ss.str());
                switch(res.size())
                {
                    case 1:res="00000"+res;break;
                    case 2:res="0000"+res;break;
                    case 3:res="000"+res;break;
                    case 4:res="00"+res;break;
                    case 5:res="0"+res;break;
                }
                fp2<<"M^"<<res<<"^05"<<endl;
            }
            string e="E^000000";
            fp2<<endl<<e;
        }
        else
        {
            if(vi[2]=="START")
            {
                string h="H^";
                switch(vi[1].size())
                {
                    case 1:h+=vi[1]+"     ^";break;
                    case 2:h+=vi[1]+"    ^";break;
                    case 3:h+=vi[1]+"   ^";break;
                    case 4:h+=vi[1]+"  ^";break;
                    case 5:h+=vi[1]+" ^";break;

                }
                h+="000000^"+loc.substr(2);
                fp2<<h<<endl;

            }
            else if((mymap.count(vi[1])>0||mymap.count(vi[1].substr(1))))
            {
                if(vi[1][0]=='+')
                {
                    if(symtab.count(vi[2])>0)
                        mod_record.insert(vi[0]);
                }

                int len=vi.size();
                obj=vi[len-1];
                flag=false;
            }
            else if(mymap.count(vi[2])>0||mymap.count(vi[2].substr(1))||vi[2]=="BYTE"||vi[2]=="WORD")
            {
                if(vi[2][0]=='+')
                {
                    if(symtab.count(vi[3])>0)
                        mod_record.insert(vi[0]);
                }

                int len=vi.size();
                obj=vi[len-1];
                flag=false;
            }
            else
            {
                flag=true;
                if(T.size()>0)
                {
                    Text_Record_Size(counter);
                    T=T.substr(0,T.size()-1);
                    transform(T.begin(), T.end(),T.begin(), ::toupper);
                    fp2<<T<<endl;

                }
                counter=0;
                T="";


            }
            if(obj.size()>0)
            {
                if(counter==0||flag)
                {
                    T="T^00"+vi[0]+"^1E^"+obj+"^";
                    counter+=obj.size()/2;

                }
                else
                {
                    if(counter+obj.size()/2<30)
                    {
                        counter+=obj.size()/2;
                        T+=obj+"^";


                    }
                    else
                    {
                        Text_Record_Size(counter);
                        T=T.substr(0,T.size()-1);
                        transform(T.begin(), T.end(),T.begin(), ::toupper);

                        fp2<<T<<endl;
                        counter=0;
                        flag=false;
                        T="T^00"+vi[0]+"^1E^"+obj+"^";
                        counter+=obj.size()/2;
                    }
                }

            }

        }
    }

}
void symtab_generation()
{

ofstream fout("Symtab.txt");
map<string, string>::iterator itr;

    for (itr = symtab.begin(); itr != symtab.end(); ++itr) {
        fout << '\t' << itr->first
             << '\t' << itr->second << '\n';
    }


fout.close();


}
int main(int argc, char const *argv[])
{
    string fn;
    fn=argv[1];
    register_map["A"]="0";
    register_map["X"]="1";
    register_map["L"]="2";
    register_map["B"]="3";
    register_map["S"]="4";
    register_map["T"]="5";
    register_map["F"]="6";
    register_map["PC"]="8";
    register_map["SW"]="9";
    binary_map["0"]="0000";
    binary_map["1"]="0001";
    binary_map["2"]="0010";
    binary_map["3"]="0011";
    binary_map["4"]="0100";
    binary_map["5"]="0101";
    binary_map["6"]="0110";
    binary_map["7"]="0111";
    binary_map["8"]="1000";
    binary_map["9"]="1001";
    binary_map["a"]="1010";
    binary_map["b"]="1011";
    binary_map["c"]="1100";
    binary_map["d"]="1101";
    binary_map["e"]="1110";
    binary_map["f"]="1111";

    reverse_binary_map["0000"]="0";
    reverse_binary_map["0001"]="1";
    reverse_binary_map["0010"]="2";
    reverse_binary_map["0011"]="3";
    reverse_binary_map["0100"]="4";
    reverse_binary_map["0101"]="5";
    reverse_binary_map["0110"]="6";
    reverse_binary_map["0111"]="7";
    reverse_binary_map["1000"]="8";
    reverse_binary_map["1001"]="9";
    reverse_binary_map["1010"]="a";
    reverse_binary_map["1011"]="b";
    reverse_binary_map["1100"]="c";
    reverse_binary_map["1101"]="d";
    reverse_binary_map["1110"]="e";
    reverse_binary_map["1111"]="f";



    make_opmap();

    loc=algorithm_pass1(fn);
   symtab_generation();
    if(!er)
    {
        algorithm_pass2();
        write_object_code();
    }





    return 0;
}
