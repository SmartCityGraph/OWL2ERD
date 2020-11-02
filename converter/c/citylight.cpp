#include <iostream>
#include <string>
#include <fstream>
#include <Windows.h>
#include <vector>
#include <stdlib.h>
#include <map>
using namespace std;

class entity
{
public:
    string name;
    string ERD_name = "";
    string label;
    string comment;
    string subclass_of;
    string reverse_property;
    string is_table;
    string range;
    string domain;
    string properties;
    string check;
    string has_category;
    string not_convertable;
    int link;
    int properties_amount;
};

entity classes_list[1000];
entity object_prop_list[1000];
entity data_prop_list[1000];
entity classes_safe_list[1000];

ofstream relation_database;
ofstream service_map;


int line_count = 1;




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
    
    cout << "======" << endl << "Opening ontology:";

    string line;
    ifstream ontology("ontology.ttl");
    getline(ontology, line);

    if (line != "") 
    {
        cout << "SUCCESS" << endl;
    }
    else
    {
        cout << "FAILED, ontology not found or wrong format. Make sure, that ontology has name \"ontology.ttl\" and first string of ontology not null";
        return 0;
    }

    map <string, string> classes_name;

    map <string, string> type;
    type["xsd:int"] = "INT ,";
    type["xsd:boolean"] = "BOOL ,";
    type["xsd:dateTimeStamp"] = "DATETIME ,";
    type["xsd:float"] = "FLOAT ,";


    
    
    string category = "category";
    int line_control;

    int objamount = 0;
    int datamount = 0;
    int clasamount = 0;


    string table_list;

    

    cout << "Type category alternative name:" << endl;
    cin >> category;
    if (category == "" || category == "default")
    {
        category = "category";
    }
    cout << endl << "=====" << endl << "Reading entities: SUCCESS" << endl;


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

        line_control = line.find("###  ");
        lines = line.find("#    Individuals");
        if (line_control > -1)
        {
            string atribut;
            string atributs[13];
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
                    atributs[12] = "1";
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
                        object_prop_list[objamount].name = atributs[1];
                        object_prop_list[objamount].domain = atribut;
                        object_prop_list[objamount].range = atributs[6];
                        object_prop_list[objamount].ERD_name = atributs[4];
                        object_prop_list[objamount].is_table = "0";
                        object_prop_list[objamount].reverse_property = atributs[9];
                        object_prop_list[objamount].label = atributs[10];
                        object_prop_list[objamount].comment = atributs[11];
                        object_prop_list[objamount].link = -1;
                        object_prop_list[objamount].properties_amount = 0;
                        object_prop_list[objamount].not_convertable = atributs[12];

                        objamount++;
                    }
                    if (atributs[0] == "owl:DatatypeProperty")
                    {
                        data_prop_list[datamount].name = atributs[1];
                        data_prop_list[datamount].domain = atribut;
                        data_prop_list[datamount].range = atributs[6];
                        data_prop_list[datamount].ERD_name = atributs[4];
                        data_prop_list[datamount].is_table = "0";
                        data_prop_list[datamount].label = atributs[10];
                        data_prop_list[datamount].comment = atributs[11];
                        data_prop_list[datamount].link = -1;
                        data_prop_list[datamount].not_convertable = atributs[12];
                        datamount++;
                    }
                }
            }

            if (atributs[0] == "owl:ObjectProperty")
            {
                object_prop_list[objamount].name = atributs[1];
                object_prop_list[objamount].domain = atributs[5];
                object_prop_list[objamount].range = atributs[6];
                object_prop_list[objamount].ERD_name = atributs[4];
                object_prop_list[objamount].is_table = "0";
                object_prop_list[objamount].reverse_property = atributs[9];
                object_prop_list[objamount].label = atributs[10];
                object_prop_list[objamount].comment = atributs[11];
                object_prop_list[objamount].link = -1;
                object_prop_list[objamount].properties_amount = 0;
                object_prop_list[objamount].not_convertable = atributs[12];

                objamount++;
            }
            if (atributs[0] == "owl:DatatypeProperty")
            {
                data_prop_list[datamount].name = atributs[1];
                data_prop_list[datamount].domain = atributs[5];
                data_prop_list[datamount].range = atributs[6];
                data_prop_list[datamount].ERD_name = atributs[4];
                data_prop_list[datamount].link = -1;
                data_prop_list[datamount].label = atributs[10];
                data_prop_list[datamount].comment = atributs[11];
                data_prop_list[datamount].not_convertable = atributs[12];
                datamount++;
            }
            if (atributs[0] == "owl:Class")
            {
                classes_list[clasamount].name = atributs[1];
                classes_list[clasamount].subclass_of = atributs[2];
                classes_list[clasamount].is_table = atributs[3];
                classes_list[clasamount].ERD_name = atributs[4];
                classes_list[clasamount].label = atributs[10];
                classes_list[clasamount].comment = atributs[11];
                classes_list[clasamount].not_convertable = atributs[12];
                
                classes_safe_list[clasamount].name = atributs[1];
                classes_safe_list[clasamount].subclass_of = atributs[2];
                classes_safe_list[clasamount].is_table = atributs[3];

                if (classes_list[clasamount].is_table == "yes")
                {
                    classes_list[clasamount].link = clasamount;
                }

                clasamount++;
            }
           
        }
    }
    cout << "Found " << clasamount << " classes: " << classes_list[0].name;
    for (int i = 1; i < clasamount; i++)
    {
        cout << ", " << classes_list[i].name;
    }
    cout << endl << endl;
    cout << "Found " << objamount << " object properties: " << object_prop_list[0].name; 
    for (int i = 1; i < objamount; i++)
    {
        cout << ", " << object_prop_list[i].name;
    }
    cout << endl << endl;
    cout << "Found " << datamount << " data properties: " << data_prop_list[0].name;
    for (int i = 1; i < datamount; i++)
    {
        cout << ", " << data_prop_list[i].name;
    }
    cout << endl << endl;

    cout << "Found annotations ERDNAME for entities: ";
    for (int i = 0; i < 1000; i++)
    {
        if (classes_list[i].ERD_name != "")
        {
            cout << classes_list[i].ERD_name << ", ";
        }
        if (object_prop_list[i].ERD_name != "")
        {
            cout << object_prop_list[i].ERD_name << ", ";
        }
        if (data_prop_list[i].ERD_name != "")
        {
            cout << data_prop_list[i].ERD_name << ", ";
        }

    }
    cout << "DONE" << endl << endl;

    cout << "Found annotations NOTCONVERTABLE for entities: ";
    for (int i = 0; i < 1000; i++)
    {
        if (classes_list[i].not_convertable != "")
        {
            cout << classes_list[i].name << ", ";
        }
        if (object_prop_list[i].not_convertable != "")
        {
            cout << object_prop_list[i].name << ", ";
        }
        if (data_prop_list[i].not_convertable != "")
        {
            cout << data_prop_list[i].name << ", ";
        }

    }
    cout << "DONE" << endl << endl;

    cout << "Found annotations REVERSEPROPERTY for entities: ";
    for (int i = 0; i < 1000; i++)
    {
        if (classes_list[i].reverse_property != "")
        {
            cout << classes_list[i].name << ", ";
        }
        if (object_prop_list[i].reverse_property != "")
        {
            cout << object_prop_list[i].name << ", ";
        }
        if (data_prop_list[i].reverse_property != "")
        {
            cout << data_prop_list[i].name << ", ";
        }

    }
    cout << "DONE" << endl << endl;
    cout << "======" << endl;

        int obj_check = 1;
        int data_check = 1;
        int class_check = 1;

        for (int i = 0; i < clasamount; i++)
        {
            for (int j = 0; j < datamount; j++)
            {
                if ((classes_list[i].name == data_prop_list[j].domain) && (classes_list[i].name != "") && (data_prop_list[j].domain != ""))
                {
                   

                    if (classes_list[i].is_table == "yes")
                    {
                        data_prop_list[j].is_table = "yes";
                        data_prop_list[j].link = i;
                    }
                    else
                    {
                        classes_list[i].properties = classes_list[i].properties + data_prop_list[j].name + "," + data_prop_list[j].range + ",";
                        classes_list[i].properties_amount++;
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
                   if ((classes_list[i].is_table == "") && (classes_list[i].subclass_of != "") && (classes_list[i].check == ""))
                   {
                       for (int j = 0; j < clasamount; j++)
                       {
                           if (classes_list[i].subclass_of == classes_safe_list[j].name)
                           {
                               classes_list[j].has_category = "yes";
                               if (classes_safe_list[j].is_table == "yes")
                               {
                                   classes_list[i].check = "1";
                                   classes_list[i].link = j;
                               }
                               else if (classes_safe_list[j].subclass_of != "")
                               {
                                   classes_list[i].subclass_of = classes_safe_list[j].subclass_of;
                               }
                               else if ((classes_safe_list[j].subclass_of == "") && (classes_safe_list[j].is_table == ""))
                               {
                                   classes_list[i].check = "1";
                               }
                               if (classes_list[i].check != "1")
                               {
                                   class_check++;
                               }
                           }
                       }
                   }
                   if ((classes_list[i].subclass_of == ""))
                   {
                       classes_list[i].check == "1";
                   }
               }
           }
           cout << "Classes change done" << endl;
           while (obj_check > 0)
           {
               obj_check = 0;
               for (int i = 0; i < objamount; i++)
               {
                   if (object_prop_list[i].name != "")
                   {
                       int domain_num;
                       int range_num;
                       for (int j = 0; j < clasamount; j++)
                       {
                           if ((classes_list[j].name == object_prop_list[i].domain) && (classes_list[j].name != "") && (object_prop_list[i].domain != ""))
                           {                               
                               domain_num = j;                             
                           }
                           if ((classes_list[j].name == object_prop_list[i].range) && (classes_list[j].name != "") && (object_prop_list[i].range != ""))
                           {                           
                               range_num = j;                               
                           }
                       }
                       if ((classes_list[domain_num].is_table == "yes") && (classes_list[range_num].is_table == "yes"))
                       {
                           object_prop_list[i].check = "1";
                           object_prop_list[i].link = range_num;
                           object_prop_list[i].is_table = "yes";
                           if (object_prop_list[i].reverse_property== "yes")
                           {
                               object_prop_list[i].link = domain_num;
                           }

                       }
                       else if ((classes_list[domain_num].is_table == "yes") && (classes_list[range_num].is_table == ""))
                       {
                          
                           object_prop_list[i].check = "2";
                           object_prop_list[i].properties = classes_list[range_num].properties;
                           object_prop_list[i].properties_amount = classes_list[range_num].properties_amount;
                           classes_list[range_num].link = domain_num;
                           object_prop_list[i].link = domain_num;

                       }
                       else if ((classes_list[domain_num].is_table == "") && (classes_list[range_num].is_table == ""))
                       {
                           object_prop_list[i].domain = classes_list[classes_list[domain_num].link].name;
                       }
                       if (object_prop_list[i].check == "0")
                       {
                           obj_check++;
                       }
                   }
               }
           }
           cout << "Object properties change done" << endl;
           cout << endl << "======";

                 ofstream service_map_three;
                 ofstream documentation;
                 documentation.open("documentation.txt");
                 service_map_three.open("tables.sql");
                 string atribut_change;
                 string primar_key;

                 cout << "Output start" << endl;
                 for (int i = 0; i < clasamount; i++)
                 {
                     if (classes_list[i].is_table == "yes")
                     {
                         if (classes_list[i].ERD_name == "")
                         {
                             string ontology_prefix = "";
                             int has;
                             int is;
                             string atribut_name = classes_list[i].name;
                             
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
                             cout << endl << "Table " << atribut_change << " create" << endl;
                         }
                         else
                         {
                             service_map_three << "CREATE TABLE " << classes_list[i].ERD_name << " (" << endl;
                             cout << endl << "Table " << classes_list[i].ERD_name << " create" << endl;
                         }
                         if (classes_list[i].ERD_name == "")
                         {
                             service_map_three << "id" << atribut_change << " INT ," << endl;
                             primar_key = "id" + atribut_change;
                         }
                         else
                         {
                             service_map_three << "id" << classes_list[i].ERD_name << " INT ," << endl;
                             primar_key = "id" + classes_list[i].ERD_name;
                         }
                         service_map_three << "label TINYTEXT ," << endl;
                         if (classes_list[i].has_category == "yes")
                         {
                             service_map_three << category <<" TINYTEXT ," << endl;
                         }
                         for (int j = 0; j < datamount; j++)
                         {
                             if (data_prop_list[j].name != "")
                             {
                    
                                 if (data_prop_list[j].link == i && data_prop_list[j].is_table == "yes")
                                 {
                                     string ontology_prefix = "";
                                     int has;
                                     int is;
                                     string atribut_name = data_prop_list[j].name;

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
                                     if (type[data_prop_list[j].range] == "")
                                     {
                                         service_map_three << " TEXT ," << endl;
                                     } 
                                     else
                                     {
                                         service_map_three << " " << type[data_prop_list[j].range] << endl;
                                     }
                                     }
                             }
                             if (object_prop_list[j].name != "")
                             {
                   
                                 if (object_prop_list[j].link == i)
                                 {
                                     if (object_prop_list[j].is_table == "yes")
                                     {
                                         string atribut_change;
                                         string ontology_prefix = "";
                                         int has;
                                         int is;
                                         string atribut_name = classes_name[object_prop_list[j].domain];
                                         if (object_prop_list[j].reverse_property == "yes")
                                         {
                                             atribut_name = classes_name[object_prop_list[j].range];
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
                                         for (int m = 1; m <= object_prop_list[j].properties_amount * 2; m++)
                                         {
                                             string ontology_prefix = "";
                                             int has;
                                             int is;
                                             string atribut_name = object_prop_list[j].name;
                                
                           

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
                                             cout << "Property " << atribut_change;

                                             ontology_prefix = "";
                                             
                                             atribut_name = atribut(m,object_prop_list[j].properties);
                       
          

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
                                             cout << atribut_change << " create." << endl;

                                             if (type[atribut(2 * m, object_prop_list[j].properties)] != "")
                                             {
                                                 service_map_three << " " << type[atribut(2 * m, object_prop_list[j].properties)]  << endl;
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
                     if (classes_list[i].label != "") {
                         documentation << "Class name: " << classes_list[i].label << endl << classes_list[i].comment << endl << endl;
                     }
                     else
                     {
                         documentation << "Class name: " << classes_list[i].name << endl << classes_list[i].comment << endl << endl;
                     }
                 }
             documentation << endl << endl << "Object properties: " << endl << endl;
             for (int i = 0; i < objamount; i++)
             {
                 if (object_prop_list[i].label != "") {
                     documentation << "Object property name: " << object_prop_list[i].label << endl << object_prop_list[i].comment << endl << endl;
                 }
                 else
                 {
                     documentation << "Object property name: " << object_prop_list[i].name << endl << object_prop_list[i].comment << endl << endl;
                 }
             }
             documentation << endl << endl << "Data properties: " << endl << endl;
             for (int i = 0; i < datamount; i++)
             {
                 if (data_prop_list[i].label != "") {
                     documentation << "Data property name: " << data_prop_list[i].label << endl << data_prop_list[i].comment << endl << endl;
                 }
                 else
                 {
                     documentation << "Data property name: " << data_prop_list[i].name << endl << data_prop_list[i].comment << endl << endl;
                 }
             }
             ontology.close();
             cout << "Output end. Files \"tables.sql\" and \"documentation.txt\" are located in the folder of program." << endl << endl;
             return 0;
}

