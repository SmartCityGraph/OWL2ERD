#include <iostream>
#include <string>
#include <fstream>
#include <Windows.h>
#include <vector>
#include <stdlib.h>
#include <map>
using namespace std;

ofstream relation_database;
ofstream service_map;

map <string, string> classes;
map <string, string> tables;


vector <string> tables_done;

int line_count = 1;


class table
{
public:
    string name_in_ontology;
    string name_table;
    boolean category;
    vector <string> atributs;
};

int char_pos(string line, char char_needed, int order)
{
    if (order == 0) {
        return -1;
    }
    int char_counter = 0;
    int quotes_count = 0;
    int brackets_count = 0;
    int i = 0;
    while (line[i] != '\0')
    {
        if (((quotes_count % 2) == 0) && ((brackets_count % 2) == 0))
        {
            if (line[i] == char_needed)
            {
                char_counter++;
            }
            if (char_counter == order)
            {
                return i;
            }
        }
        if (line[i] == '\"')
        {
            quotes_count++;
        }
        if ((line[i] == '(') || (line[i] == ')'))
        {
            brackets_count++;
        }
        i++;
    }
    return line.length();
}

string atribut(int atr_num, string line)
{
    int first_char = char_pos(line, ',', atr_num - 1) + 1;
    int second_char = char_pos(line, ',', atr_num);
    line = line.substr(first_char, (second_char - first_char));
    if (line.length() == 0)
    {
        return "";
    }
    else
    {
        return line;
    }
}

int main()
{
    cout << "Opening ontology" << endl;
    ifstream ontology("ontology.ttl");
    cout << "Ontology open" << endl;

    map <string, string> classes_name;
    map <string, string> type;
    type["xsd:int"] = "INT ,";
    type["xsd:boolean"] = "BOOL ,";
    type["xsd:dateTimeStamp"] = "DATETIME ,";
    type["xsd:float"] = "FLOAT ,";


    
    string line;
    string category = "category";
    int liness;

    int objamount = 0;
    int datamount = 0;
    int clasamount = 0;

    string object_property_list[100][12];
    string data_property_list[100][12];
    string classes_list[100][12];
    string classes_list_safe[100][8];
    string table_list;

    int classes_link_list[100][2];
    int classes_link_list_safe[100];
    int object_property_link_list[100][2];
    int data_property_link_list[100];
    
    cout << "Type category alternative name:" << endl;
    cin >> category;
    if (category == "" || category == "default")
    {
        category = "category";
    }
    cout << "Reading entities" << endl;
    getline(ontology, line);
    int lines = line.find("#    Object Properties");
    while (lines < 0)
    {
        getline(ontology, line);
        lines = line.find("#    Object Properties");
    }
    lines = line.find("#    Individuals");
    while (lines < 0)
    {
        getline(ontology, line);

        liness = line.find("###  ");
        lines = line.find("#    Individuals");
        if (liness > -1)
        {
            string line = "!";
            string atribut;
            string atributs[12];
            int atribut_begin;
            int atribut_end;
            int lines;
            atributs[3] = "yes";
            atributs[4] = "";
            while (line != "")
            {
                getline(ontology, line);
                lines = line.find("rdf:type");
                if (lines > -1)
                {
                    

                    atribut_begin = 0;
                    atribut_end = char_pos(line, ' ', 1);
                    atribut = line.substr(atribut_begin, atribut_end - atribut_begin);
                    
                    atributs[1] = atribut;
                    classes_name[atribut] = atribut;
                    atribut_begin = char_pos(line, ' ', 2)+1;
                    atribut_end = char_pos(line, ' ', 3);
                    atribut = line.substr(atribut_begin, atribut_end - atribut_begin);
                    atributs[0] = atribut;
                    
                    

                }
                
                
       
                lines = line.find("subClassOf");
                if (lines > -1)
                {
                    atribut_begin = line.find("subClassOf") + 11;
                    atribut_end = line.length() - 2;
                    atribut = line.substr(atribut_begin, atribut_end - atribut_begin);
                    atributs[2] = atribut;
                    atributs[3] = "";
                }
                    lines = line.find("ERDname");
                    if (lines > -1)
                    {
                        atribut_begin = line.find("ERDname") + 9;
                        atribut_end = line.length() - 3;
                        atribut = line.substr(atribut_begin, atribut_end - atribut_begin);
                        atributs[4] = atribut;
                        classes_name[atributs[1]] = atribut;

                    }
                    lines = line.find("reverseProperty");
                    if (lines > -1)
                    {
                        atributs[9] = "yes";
                    }
                    lines = line.find("rdfs:label");
                    if (lines > -1)
                    {
                        atributs[10] = "";
                        atribut_begin = line.find("\"");
                        atribut_end = line.find("\"", atribut_begin + 1);
                        atribut = line.substr(atribut_begin, atribut_end - atribut_begin+1);
                        atributs[10] = atribut;
                    }
                    lines = line.find("rdfs:comment");
                    if (lines > -1)
                    {
                        atributs[11] = "";
                        atribut_begin = line.find("\"");
                        atribut_end = line.length()-3;
                        atribut = line.substr(atribut_begin, atribut_end - atribut_begin+1);
                        atributs[11] = atribut;
                    }
                
                lines = line.find("notConvertable");
                if (lines > -1)
                {
                    atributs[3] = "";
                }
                lines = line.find("domain");
                if (lines > -1)
                {
                    atribut_begin = line.find("domain") + 7;
                    atribut_end = line.length() - 2;
                    atribut = line.substr(atribut_begin, atribut_end - atribut_begin);
                    atributs[5] = atribut;
                }
                lines = line.find("range");
                if (lines > -1)
                {
                    atribut_begin = line.find("range") + 6;
                    atribut_end = line.length() - 2;
                    atribut = line.substr(atribut_begin, atribut_end - atribut_begin);
                    atributs[6] = atribut;
                }
                lines = line.find("areDomains");
                if (lines > -1)
                {
                    atribut_begin = line.find("areDomains") + 12;
                    atribut_end = line.length() - 3;
                    atribut = line.substr(atribut_begin, atribut_end - atribut_begin);
                    
                    if (atributs[0] == "owl:ObjectProperty")
                    {
                        object_property_list[objamount][0] = atributs[1];
                        object_property_list[objamount][1] = atribut;
                        object_property_list[objamount][2] = atributs[6];
                        object_property_list[objamount][3] = atributs[4];
                        object_property_list[objamount][4] = "0";
                        object_property_list[objamount][5] = "";
                        object_property_list[objamount][6] = "";
                        object_property_list[objamount][9] = atributs[9];
                        object_property_list[clasamount][10] = atributs[10];
                        object_property_list[clasamount][11] = atributs[11];
                        object_property_link_list[objamount][0] = -1;
                        object_property_link_list[objamount][1] = 0;

                        objamount++;
                    }
                    if (atributs[0] == "owl:DatatypeProperty")
                    {
                        data_property_list[datamount][0] = atributs[1];
                        data_property_list[datamount][1] = atribut;
                        data_property_list[datamount][2] = atributs[6];
                        data_property_list[datamount][3] = atributs[4];
                        data_property_list[datamount][4] = "0";
                        data_property_list[datamount][5] = "";
                        data_property_list[clasamount][10] = atributs[10];
                        data_property_list[clasamount][11] = atributs[11];
                        data_property_link_list[datamount] = -1;
                        datamount++;
                    }
                }
            }

            if (atributs[0] == "owl:ObjectProperty")
            {
                object_property_list[objamount][0] = atributs[1];
                object_property_list[objamount][1] = atributs[5];
                object_property_list[objamount][2] = atributs[6];
                object_property_list[objamount][3] = atributs[4];
                object_property_list[objamount][4] = "0";
                object_property_list[objamount][5] = "";
                object_property_list[objamount][6] = "";
                object_property_list[objamount][9] = atributs[9];
                object_property_list[clasamount][10] = atributs[10];
                object_property_list[clasamount][11] = atributs[11];
                object_property_link_list[objamount][0] = -1;
                object_property_link_list[objamount][1] = 0;

                objamount++;
            }
            if (atributs[0] == "owl:DatatypeProperty")
            {
                data_property_list[datamount][0] = atributs[1];
                data_property_list[datamount][1] = atributs[5];
                data_property_list[datamount][2] = atributs[6];
                data_property_list[datamount][3] = atributs[4];
                data_property_list[datamount][4] = "0";
                data_property_list[datamount][5] = "";
                data_property_link_list[datamount] = -1;
                data_property_list[clasamount][10] = atributs[10];
                data_property_list[clasamount][11] = atributs[11];
                datamount++;
            }
            if (atributs[0] == "owl:Class")
            {
                classes_list[clasamount][0] = atributs[1];
                classes_list[clasamount][1] = atributs[2];
                classes_list[clasamount][2] = atributs[3];
                classes_list[clasamount][3] = atributs[4];
                classes_list[clasamount][4] = atributs[5];
                classes_list[clasamount][5] = "";
                classes_list[clasamount][6] = "";
                classes_list[clasamount][9] = "";
                classes_list[clasamount][10] = atributs[10];
                classes_list[clasamount][11] = atributs[11];
                
                classes_list_safe[clasamount][0] = atributs[1];
                classes_list_safe[clasamount][1] = atributs[2];
                classes_list_safe[clasamount][2] = atributs[3];
                classes_list_safe[clasamount][3] = atributs[4];
                classes_list_safe[clasamount][4] = atributs[5];
                classes_list_safe[clasamount][5] = "";
                classes_list_safe[clasamount][6] = "";
                classes_link_list[clasamount][0] = clasamount;
                classes_link_list[clasamount][1] = 0;
                classes_link_list_safe[clasamount] = clasamount;

                if (classes_list[clasamount][2] == "yes")
                {
                    classes_list[clasamount][4] = "1";
                    classes_list_safe[clasamount][4] = "1";
                    classes_link_list[clasamount][0] = clasamount;
                    classes_link_list_safe[clasamount] = clasamount;
                }
                if (classes_list[clasamount][3] == "yes")
                {
                    classes_list[clasamount][4] = "1";
                    classes_list_safe[clasamount][4] = "1";
                }
                clasamount++;
            }
           
        }
    }
    cout << "Entities readed" << endl;
    cout << "Readed " << clasamount << " classes" << endl << "Readed " << objamount << " object properties" << endl << "Readed " << datamount << " data properties" << endl;
  

        int obj_check = 1;
        int data_check = 1;
        int class_check = 1;
        int fix;
        for (int i = 0; i < clasamount; i++)
        {
            for (int j = 0; j < datamount; j++)
            {
                if ((classes_list[i][0] == data_property_list[j][1]) && (classes_list[i][0] != "") && (data_property_list[j][1] != ""))
                {
                   

                    if (classes_list[i][2] == "yes")
                    {
                        data_property_list[j][3] = "yes";
                        data_property_link_list[j] = i;
                    }
                    else
                    {
                        classes_list[i][6] = classes_list[i][6] + data_property_list[j][0] + "," + data_property_list[j][2] + ",";
                        classes_link_list[i][1]++;
                    }
                }
            }
        }
        cout << "Data properties change done" << endl;
           while (class_check > 0)
           {
               
               class_check = 0;
               for (int i = 0; i < clasamount; i++)
               {
                   if ((classes_list[i][2] == "") && (classes_list[i][1] != "") && (classes_list[i][4] == ""))
                   {
                       for (int j = 0; j < clasamount; j++)
                       {
                           if (classes_list[i][1] == classes_list_safe[j][0])
                           {
                               classes_list[j][9] = "yes";
                               if (classes_list_safe[j][2] == "yes")
                               {
                                   classes_list[i][4] = "1";
                                   fix = j;
                                   classes_list[i][5] = fix;
                                   classes_link_list[i][0] = j;
                               }
                               else if (classes_list_safe[j][1] != "")
                               {
                                   classes_list[i][1] = classes_list_safe[j][1];
                               }
                               else if ((classes_list_safe[j][1]== "") && (classes_list_safe[j][2] == ""))
                               {
                                   classes_list[i][4] = "1";
                               }
                               if (classes_list[i][4] != "1")
                               {
                                   class_check++;
                               }
                           }
                       }
                   }
                   if ((classes_list[i][1] == ""))
                   {
                       classes_list[i][4] == "1";
                   }
               }
           }
           cout << "Classes change done" << endl;
           while (obj_check > 0)
           {
               obj_check = 0;
               for (int i = 0; i < objamount; i++)
               {
                   if (object_property_list[i][0] != "")
                   {
                       int domain_num;
                       int range_num;
                       for (int j = 0; j < clasamount; j++)
                       {
                           if ((classes_list[j][0] == object_property_list[i][1]) && (classes_list[j][0] != "") && (object_property_list[i][1] != ""))
                           {
                               
                               domain_num = j;
                               
                           }
                           if ((classes_list[j][0] == object_property_list[i][2]) && (classes_list[j][0] != "") && (object_property_list[i][2] != ""))
                           {
                               
                               range_num = j;
                               
                           }
                       }
                       if ((classes_list[domain_num][2] == "yes") && (classes_list[range_num][2] == "yes"))
                       {
                           object_property_list[i][4] = "1";
                           object_property_list[i][5] = range_num;
                           object_property_link_list[i][0] = range_num;
                           object_property_list[i][6] = "yes";
                           if (object_property_list[i][9] == "yes")
                           {
                               object_property_list[i][5] = domain_num;
                               object_property_link_list[i][0] = domain_num;
                           }

                       }
                       else if ((classes_list[domain_num][2] == "yes") && (classes_list[range_num][2] == ""))
                       {
                          
                           object_property_list[i][4] = "2";
                           classes_list[range_num][5] = domain_num;
                           object_property_list[i][6] = classes_list[range_num][6];
                          
                           object_property_link_list[i][1] = classes_link_list[range_num][1];
                           classes_link_list[range_num][0] = domain_num;
                           object_property_list[i][5] = domain_num;
                           object_property_link_list[i][0] = domain_num;

                       }
                       else if ((classes_list[domain_num][2] == "") && (classes_list[range_num][2] == ""))
                       {
                           object_property_list[i][1] = classes_list[classes_link_list[domain_num][0]][0];

                       }
                       if (object_property_list[i][4] == "0")
                       {
                           obj_check++;

                       }
                   }
               }
           }
           cout << "Object properties change done" << endl;
        

                 ofstream service_map_three;
                 ofstream documentation;
                 documentation.open("documentation.txt");
                 service_map_three.open("tables.sql");
                 string atribut_change;
                 string primar_key;

                 cout << "Output start" << endl;
                 for (int i = 0; i < clasamount; i++)
                 {
                     if (classes_list[i][2] == "yes")
                     {
                         if (classes_list[i][3] == "")
                         {
                             string ontology_prefix = "";
                             int has;
                             int is;
                             string atribut_name = classes_list[i][0];
                             
                             if (char_pos(atribut_name, ':', 1) != 0)
                             {
                                 ontology_prefix = atribut_name.substr(0, char_pos(atribut_name, ':', 1)) + "_";
                              
                             }
                             int tap_begin = char_pos(atribut_name, ':', 1)+1;
                             int tap_length = (atribut_name.length() - (tap_begin));
                             atribut_change = atribut_name.substr(tap_begin, tap_length);
                           

                             has = atribut_change.find("has");
                             is = atribut_change.find("is");
                             if (has == 0)
                             {
                                 atribut_change = atribut_change.substr(3, atribut_change.length() - 3);
                             }
                             if (is == 0)
                             {
                                 atribut_change = atribut_change.substr(2, atribut_change.length() - 2);
                             }
                             atribut_change = "CL_" + ontology_prefix + atribut_change;
                 
                             service_map_three << "CREATE TABLE " << atribut_change << " (" << endl;
                         }
                         else
                         {
                             service_map_three << "CREATE TABLE " << classes_list[i][3] << " (" << endl;
                         }
                         if (classes_list[i][3] == "")
                         {
                             service_map_three << "id" << atribut_change << " INT ," << endl;
                             primar_key = "id" + atribut_change;
                         }
                         else
                         {
                             service_map_three << "id" << classes_list[i][3] << " INT ," << endl;
                             primar_key = "id" + classes_list[i][3];
                         }
                         service_map_three << "label TINYTEXT ," << endl;
                         if (classes_list[i][9] == "yes")
                         {
                             service_map_three << category <<" TINYTEXT ," << endl;
                         }
                         for (int j = 0; j < datamount; j++)
                         {
                             if (data_property_list[j][0] != "")
                             {
                    
                                 if (data_property_link_list[j] == i && data_property_list[j][3] == "yes")
                                 {
                                     string ontology_prefix = "";
                                     int has;
                                     int is;
                                     string atribut_name = data_property_list[j][0];

                                     if (char_pos(atribut_name, ':', 1) != 0)
                                     {
                                         ontology_prefix = atribut_name.substr(0, char_pos(atribut_name, ':', 1)) + "_";
                   
                                     }
                                     int tap_begin = char_pos(atribut_name, ':', 1) + 1;
                                     int tap_length = (atribut_name.length() - (tap_begin));
                                     string atribut_change = atribut_name.substr(tap_begin, tap_length);
            

                                     has = atribut_change.find("has");
                                     is = atribut_change.find("is");
                                     if (has == 0)
                                     {
                                         atribut_change = atribut_change.substr(3, atribut_change.length() - 3);
                                     }
                                     if (is == 0)
                                     {
                                         atribut_change = atribut_change.substr(2, atribut_change.length() - 2);
                                     }
                                     atribut_change = "CL_" + ontology_prefix + atribut_change;
         
                                     service_map_three << atribut_change;
                                     if (type[data_property_list[j][2]] == "")
                                     {
                                         service_map_three << " TEXT ," << endl;
                                     } 
                                     else
                                     {
                                         service_map_three << " " << type[data_property_list[j][2]] << endl;
                                     }
                                     }
                             }
                             if (object_property_list[j][0] != "")
                             {
                   
                                 if (object_property_link_list[j][0] == i)
                                 {
                                     if (object_property_list[j][6] == "yes")
                                     {
                                         string atribut_change;
                                         string ontology_prefix = "";
                                         int has;
                                         int is;
                                         string atribut_name = classes_name[object_property_list[j][1]];
                                         if (object_property_list[j][9] == "yes")
                                         {
                                             atribut_name = classes_name[object_property_list[j][2]];
                                         }
                                         int l = atribut_name.find(":", 0);
                                         
                                             if (l != -1)
                                             { 
                               

                                                 if (char_pos(atribut_name, ':', 1) != 0)
                                                 {
                                                     ontology_prefix = atribut_name.substr(0, char_pos(atribut_name, ':', 1)) + "_";
                                       
                                                 }
                                             int tap_begin = char_pos(atribut_name, ':', 1) + 1;
                                                 int tap_length = (atribut_name.length() - (tap_begin));
                                                 atribut_change = atribut_name.substr(tap_begin, tap_length);
                  

                                             has = atribut_change.find("has");
                                             is = atribut_change.find("is");
                                             if (has == 0)
                                             {
                                                 atribut_change = atribut_change.substr(3, atribut_change.length() - 3);
                                             }
                                             if (is == 0)
                                             {
                                                 atribut_change = atribut_change.substr(2, atribut_change.length() - 2);
                                             }
                                         }
                                         else
                                         {
                                             atribut_change = atribut_name;
                                         }
                                         atribut_change = ontology_prefix + atribut_change + "_id" + "CL_" + ontology_prefix + atribut_change + " INT ,";

                                         service_map_three << "CL_" << atribut_change << endl;
                                     }
                                     else
                                     {
                                         for (int m = 1; m <= object_property_link_list[j][1]*2; m++)
                                         {
                                             string ontology_prefix = "";
                                             int has;
                                             int is;
                                             string atribut_name = object_property_list[j][0];
                                
                           

                                             if (char_pos(atribut_name, ':', 1) != 0)
                                             {
                                                 ontology_prefix = atribut_name.substr(0, char_pos(atribut_name, ':', 1)) + "_";
                      
                                             }
                                             int tap_begin = char_pos(atribut_name, ':', 1) + 1;
                                             int tap_length = (atribut_name.length() - (tap_begin));
                                             string atribut_change = atribut_name.substr(tap_begin, tap_length);
   

                                             has = atribut_change.find("has");
                                             is = atribut_change.find("is");
                                             if (has == 0)
                                             {
                                                 atribut_change = atribut_change.substr(3, atribut_change.length() - 3);
                                             }
                                             if (is == 0)
                                             {
                                                 atribut_change = atribut_change.substr(2, atribut_change.length() - 2);
                                             }
                                             atribut_change = "CL_" + ontology_prefix + atribut_change;
                                             service_map_three << atribut_change;

                                             ontology_prefix = "";
                                             
                                             atribut_name = atribut(m,object_property_list[j][6]);
                       
          

                                             if (char_pos(atribut_name, ':', 1) != 0)
                                             {
                                                 ontology_prefix = atribut_name.substr(0, char_pos(atribut_name, ':', 1)) + "_";
           
                                             }
                                             tap_begin = char_pos(atribut_name, ':', 1) + 1;
                                             tap_length = (atribut_name.length() - (tap_begin));
                                             atribut_change = atribut_name.substr(tap_begin, tap_length);
                 

                                             has = atribut_change.find("has");
                                             is = atribut_change.find("is");
                                             if (has == 0)
                                             {
                                                 atribut_change = atribut_change.substr(3, atribut_change.length() - 3);
                                             }
                                             if (is == 0)
                                             {
                                                 atribut_change = atribut_change.substr(2, atribut_change.length() - 2);
                                             }
                                             atribut_change = "_" + ontology_prefix + atribut_change;
                                             service_map_three << atribut_change;
                                             if (type[atribut(2 * m, object_property_list[j][6])] != "")
                                             {
                                                 service_map_three << " " << type[atribut(2 * m, object_property_list[j][6])]  << endl;
                                             }
                                             else
                                             {
                                                 service_map_three << " TEXT ," << endl;
                                             }
                                             m++;
                                         }
                                     }
                                 }
                             }
                         }
                         service_map_three << "PRIMARY KEY(" + primar_key + ")" << endl;
                         service_map_three << ");" << endl;
                     }
                 }

                 documentation << "Classes: " << endl << endl;
                 for (int i = 0; i < clasamount; i++)
                 {
                     if (classes_list[i][10] != "") {
                         documentation << "Class name: " << classes_list[i][10] << endl << classes_list[i][11] << endl << endl;
                     }
                     else
                     {
                         documentation << "Class name: " << classes_list[i][0] << endl << classes_list[i][11] << endl << endl;
                     }
                 }
             documentation << endl << endl << "Object properties: " << endl << endl;
             for (int i = 0; i < objamount; i++)
             {
                 if (object_property_list[i][10] != "") {
                     documentation << "Object property name: " << object_property_list[i][10] << endl << object_property_list[i][11] << endl << endl;
                 }
                 else
                 {
                     documentation << "Object property name: " << object_property_list[i][0] << endl << object_property_list[i][11] << endl << endl;
                 }
             }
             documentation << endl << endl << "Data properties: " << endl << endl;
             for (int i = 0; i < datamount; i++)
             {
                 if (data_property_list[i][10] != "") {
                     documentation << "Data property name: " << data_property_list[i][10] << endl << data_property_list[i][11] << endl << endl;
                 }
                 else
                 {
                     documentation << "Data property name: " << data_property_list[i][0] << endl << data_property_list[i][11] << endl << endl;
                 }
             }
             ontology.close();
             cout << "Output end";
             return 0;
}

