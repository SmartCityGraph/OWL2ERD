#include <iostream>
#include <string>
#include <fstream>
#include <stdlib.h>
#include <map>
#include <vector>

using namespace std;
// Описание класса сущности онтологии и перечисление внутренних переменных

class entity
{
public:
    string entity_type;
    string name;                // Имя в онтологии
    string ERD_name = "";       // Имя для базы данных
    string label = "";          // Лэйбл в онтологии
    string comment;             // Комментарий в онтологии
    string subclass_of;         // Подклассом чего является
    string clear_name;
    string range;               // Область значения для отношения
    string domain;              // Область определения для отношения
    string properties;          // Список литеральных отношений на которые отсылает сущность 
    string check = "0";         //
    bool reverse_property = 0;  // Имеет ли свойство "reverseProperty"
    bool is_table = 1;          // Является ли таблицей
    bool has_category;          // Имеет ли данный класс подклассы
    bool not_convertable = 0;       // Имеется ли свойство not_convertable, которое определяет конвертируется ли сущность
                                // в таблицу (для классов) и конвертируется ли вообще (для отношений)
    int link = -1;              // Номер класса в массиве, на который ссылается сущность 
    int properties_amount;      // Количество литеральных отношений, на которые ссылается сущность
    int domain_num;
    int range_num;
};



// Списки сущностей - классов, объектных отношений, литеральных отношений и контрольный список для классов,
// который не изменяется
vector <entity> classes_list;
vector <entity> object_prop_list;
vector <entity> data_prop_list;
vector <entity> classes_safe_list;

// Метод для поиска необходимого вхождения необходимого символа
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

// Метод для вычленения необходимой подстроки из списка, разделённого запятыми
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

string clean_name(string name)
{
    string clear_name;
    string ontology_prefix = "";
    int support_word;
   
    if (char_pos(name, ':', 1) != 0)
    {
        ontology_prefix = name.substr(0, char_pos(name, ':', 1)) + "_";
    }
    support_word = name.find(":");
    if (support_word > -1)
    {
        int tap_begin = char_pos(name, ':', 1) + 1;
        int tap_length = (name.length() - (tap_begin));
        clear_name = name.substr(tap_begin, tap_length);
    }
    else
    {
        clear_name = name;
        return clear_name;
    }
    support_word = clear_name.find("has");
    if (support_word == 0)
    {
        clear_name = clear_name.substr(3, clear_name.length() - 3);
    }
    support_word = clear_name.find("is");
    if (support_word == 0)
    {
        clear_name = clear_name.substr(2, clear_name.length() - 2);
    }
    clear_name = "CL_" + ontology_prefix + clear_name;
    return clear_name;
}

// Основной метод
int main()
{
    // Открытие онтологии, вывод ошибки в случае неправильного формата или имени файла
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

    // Создание вспомогательных переменных и карт - карту имён сущностей и карту типов переменных из ttl в sql
    map <string, string> classes_name_map;
    map <string, string> type_map;
    type_map["xsd:int"] = "INT ,";
    type_map["xsd:boolean"] = "BOOL ,";
    type_map["xsd:dateTimeStamp"] = "DATETIME ,";
    type_map["xsd:float"] = "FLOAT ,";
    // Счётчики числа тех или иных сущностей
    int objamount = 0;
    int datamount = 0;
    int clasamount = 0;

    // Запрос на имя для свойства категории. В случае введения "default", свойство категории будет иметь стандартное 
    // имя "category"
    string category = "category";
    cout << "Type category alternative name:" << endl;
    cin >> category;
    if (category == "default")
    {
        category = "category";
    }
    
    ///////////////////////////////////////////////////////////////////////////////////
    // Начало блока считывания сущностей. 
    // Переменная entity_finder хранит число отображающее вхождение подстроки "###" которая объявляет новую сущность. 
    // Переменная line_limiter хранит число отображающее вхождение сперва подстроки
    // "#    Object Properties", после которой идут интересующие нас сущности, а затем подстроки "#    Individuals",
    // после которой интересующие нас сущности заканчиваются.
    int entity_finder;
    int line_limiter = line.find("#    Object Properties");
    while (line_limiter < 0)
    {
        getline(ontology, line);
        line_limiter = line.find("#    Object Properties");
    }
    line_limiter = line.find("#    Individuals");
    while (line_limiter < 0)
    {
        getline(ontology, line);
        entity_finder = line.find("###  ");
        line_limiter = line.find("#    Individuals");

        if (entity_finder > -1)
        {
            // Если программа находит необходимую подстроку, она начинает создание новой сущности внутри программы
            // на основании сущности из онтологии, до тех пор, пока сущность в онтологии не закончится. 
            // После того как это случится, программа закончит создание сущности и начнёт поиск новой.
            string atribut;
            entity temp_entity;
            int atribut_begin;
            int atribut_end;
            int entity_properties_finder;

            while (line != "")
            {
                getline(ontology, line);
                // Если мы встречаем вхождение подстроки "rdf:type" то мы определяем имя сущности, а также её тип
                entity_properties_finder = line.find("rdf:type");
                if (entity_properties_finder > -1)
                {
                    atribut_begin = 0;
                    atribut_end = char_pos(line, ' ', 1);
                    atribut = line.substr(atribut_begin, atribut_end - atribut_begin);
                    temp_entity.name = atribut;

                    classes_name_map[atribut] = atribut;

                    atribut_begin = char_pos(line, ' ', 2) + 1;
                    atribut_end = char_pos(line, ' ', 3);
                    atribut = line.substr(atribut_begin, atribut_end - atribut_begin);
                    temp_entity.entity_type = atribut;
                    temp_entity.clear_name = clean_name(temp_entity.name);

                }

                // Если мы встречаем вхождение подстроки "subClassOf" то мы определяем подклассом какого класса
                // является сущность, а также указываем, что она не является таблицей
                entity_properties_finder = line.find("subClassOf");
                if (entity_properties_finder > -1)
                {
                    atribut_begin = line.find("subClassOf") + 11;
                    atribut_end = line.length() - 2;
                    atribut = line.substr(atribut_begin, atribut_end - atribut_begin);
                    temp_entity.subclass_of = atribut;
                    temp_entity.is_table = 0;
                }

                // Если мы встречаем вхождение подстроки "ERDname" то мы определяем ERD имя сущности
                // А также указываем соответствие её реального имени и ERD имени в карте имён
                entity_properties_finder = line.find("ERDname");
                if (entity_properties_finder > -1)
                {
                    atribut_begin = line.find("ERDname") + 9;
                    atribut_end = line.length() - 3;
                    atribut = line.substr(atribut_begin, atribut_end - atribut_begin);
                    temp_entity.ERD_name = atribut;
                    temp_entity.clear_name = clean_name(temp_entity.ERD_name);
                    classes_name_map[temp_entity.name] = atribut;
                }

                // Если мы встречаем вхождение подстроки "reverseProperty" то мы определяем имеет ли сущность
                // одноимённое свойство
                entity_properties_finder = line.find("reverseProperty");
                if (entity_properties_finder > -1)
                {
                    temp_entity.reverse_property = 1;
                }

                // Если мы встречаем вхождение подстроки "rdfs:label" то мы определяем лэйбл сущности
                entity_properties_finder = line.find("rdfs:label");
                if (entity_properties_finder > -1)
                {
                    cout << temp_entity.label << endl;
                    atribut_begin = line.find("\"");
                    atribut_end = line.find("\"", atribut_begin + 1);
                    atribut = line.substr(atribut_begin, atribut_end - atribut_begin + 1);
                    temp_entity.label = atribut;
                    cout << temp_entity.label << endl;
                }

                // Если мы встречаем вхождение подстроки "rdfs:comment" то мы определяем комментарии сущности для документации
                entity_properties_finder = line.find("rdfs:comment");
                if (entity_properties_finder > -1)
                {
                    atribut_begin = line.find("\"");
                    atribut_end = line.length() - 3;
                    atribut = line.substr(atribut_begin, atribut_end - atribut_begin + 1);
                    temp_entity.comment = temp_entity.comment + '\n' + atribut;
                }

                // Если мы встречаем вхождение подстроки "notConvertable" то мы определяем имеется ли у сущности
                // одноимённое свойство, а также указываем, что сущность не является таблицей (для классов) или игнорируется
                // при конвертации (для отношений)
                entity_properties_finder = line.find("notConvertable");
                if (entity_properties_finder > -1)
                {
                    temp_entity.is_table = 0;
                    temp_entity.not_convertable = 1;
                }

                // Если мы встречаем вхождение подстроки "domain" то мы определяем имеется ли у сущности область определения
                entity_properties_finder = line.find("domain");
                if (entity_properties_finder > -1)
                {
                    atribut_begin = line.find("domain") + 7;
                    atribut_end = line.length() - 2;
                    atribut = line.substr(atribut_begin, atribut_end - atribut_begin);
                    temp_entity.domain = atribut;
                }

                // Если мы встречаем вхождение подстроки "range" то мы определяем имеется ли у сущности область значений
                entity_properties_finder = line.find("range");
                if (entity_properties_finder > -1)
                {
                    atribut_begin = line.find("range") + 6;
                    atribut_end = line.length() - 2;
                    atribut = line.substr(atribut_begin, atribut_end - atribut_begin);
                    temp_entity.range = atribut;
                }

                // Если мы встречаем вхождение подстроки "areDomains" то мы создаём новую сущность с теми же параметрами,
                // что и изначальная, но с другой областью определения, для случаев, когда у одной сущности несколько
                // классов области определения
                entity_properties_finder = line.find("areDomains");
                if (entity_properties_finder > -1)
                {
                    atribut_begin = line.find("areDomains") + 12;
                    atribut_end = line.length() - 3;
                    atribut = line.substr(atribut_begin, atribut_end - atribut_begin);
                    if (temp_entity.entity_type == "owl:ObjectProperty")
                    {
                        entity second_temp_entity = temp_entity;
                        second_temp_entity.domain = atribut;
                        second_temp_entity.is_table = "0";
                        second_temp_entity.properties_amount = 0;
                        object_prop_list.push_back(second_temp_entity);
                        objamount++;
                    }
                    if (temp_entity.entity_type == "owl:DatatypeProperty")
                    {
                        entity second_temp_entity = temp_entity;
                        second_temp_entity.domain = atribut;
                        second_temp_entity.is_table = "0";
                        second_temp_entity.properties_amount = 0;
                        object_prop_list.push_back(second_temp_entity);
                        datamount++;
                    }
                }

            }
        
                // После всех проверок мы заканчиваем создние сущности, и в зависимости от её типа, оптравляем её в один
                // или несколько из четырёх векторов. Объектные отношения отправляются в object_prop_list,
                // литеральные отношения в data_prop_list, а классы в classes_list и classes_safe_list
                if (temp_entity.entity_type == "owl:ObjectProperty")
                {
                    temp_entity.is_table = "0";
                    temp_entity.properties_amount = 0;
                    object_prop_list.push_back(temp_entity);
                    objamount++;
                }
                if (temp_entity.entity_type == "owl:DatatypeProperty")
                {
                    data_prop_list.push_back(temp_entity);
                    datamount++;
                }
                if (temp_entity.entity_type == "owl:Class")
                {
                    if (temp_entity.is_table == 1)
                    {
                        temp_entity.link = clasamount;
                    }
                    classes_list.push_back(temp_entity);
                    classes_safe_list.push_back(temp_entity);
                    clasamount++;
                }

            
        }
    }

            cout << endl << "=====" << endl << "Reading entities: SUCCESS" << endl;
            // Заканчиваем чтение сущностей и выводим некоторую полезную информацию в консоль, количество найденых сущностей
            // а также сущностей с некоторыми свойствами

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
            for (int i = 0; i < clasamount; i++)
            {
                if (classes_list[i].ERD_name != "")
                {
                    cout << classes_list[i].ERD_name << ", ";
                }
            }
            for (int i = 0; i < objamount; i++)
            {
                if (object_prop_list[i].ERD_name != "")
                {
                    cout << object_prop_list[i].ERD_name << ", ";
                }
            }
            for (int i = 0; i < datamount; i++)
            {
                if (data_prop_list[i].ERD_name != "")
                {
                    cout << data_prop_list[i].ERD_name << ", ";
                }

            }
            cout << "DONE" << endl << endl;

            cout << "Found annotations NOTCONVERTABLE for entities: ";
            for (int i = 0; i < clasamount; i++)
            {
                if (classes_list[i].not_convertable != 0)
                {
                    cout << classes_list[i].name << ", ";
                }
            }
            for (int i = 0; i < objamount; i++)
            {
                if (object_prop_list[i].not_convertable != 0)
                {
                    cout << object_prop_list[i].name << ", ";
                }
            }
            for (int i = 0; i < datamount; i++)
            {
                if (data_prop_list[i].not_convertable != 0)
                {
                    cout << data_prop_list[i].name << ", ";
                }
            }
            cout << "DONE" << endl << endl;

            cout << "Found annotations REVERSEPROPERTY for entities: ";
            for (int i = 0; i < objamount; i++)
            {
                if (object_prop_list[i].reverse_property != 0)
                {
                    cout << object_prop_list[i].name << ", ";
                }
            }
            for (int i = 0; i < datamount; i++)
            {
                if (data_prop_list[i].reverse_property != 0)
                {
                    cout << data_prop_list[i].name << ", ";
                }
            }
            cout << "DONE" << endl << endl;
            cout << "======" << endl;
            // Конец блока считывания сущностей
            ///////////////////////////////////////////////////////////////////////////////////////////////

            ///////////////////////////////////////////////////////////////////////////////////////////////
            // Начала блока обработки сущностей
            int obj_check = 1;     // контрольное число для проверки правильности обработки объектных отношений
            int class_check = 1;   // контрольное число для проверки правильности обработки классов

            // Проверка всех литеральных отношений, если они относятся к классу таблицы - литеральное отношение получает 
            // ссылку на эту таблицу. В противном случае, класс получает это литеральное отношение и его тип 
            // в свой пул литеральных отношений
            for (int i = 0; i < clasamount; i++)
            {
                for (int j = 0; j < datamount; j++)
                {
                    if ((classes_list[i].name == data_prop_list[j].domain) && (classes_list[i].name != "") && (data_prop_list[j].domain != ""))
                    {
                        if (classes_list[i].is_table == 1)
                        {
                            data_prop_list[j].is_table = 1;
                            data_prop_list[j].link = i;
                        }
                        else
                        {
                            classes_list[i].properties = classes_list[i].properties + data_prop_list[j].clear_name + "," + data_prop_list[j].range + ",";
                            classes_list[i].properties_amount++;
                        }
                    }
                }
            }
            cout << "Data properties change done" << endl;

            // Проверка всех классов. Все подклассы получают ссылку на верхнеуровневый родительский класс 
            while (class_check > 0)
            {
                class_check = 0;
                for (int i = 0; i < clasamount; i++)
                {
                    if ((classes_list[i].is_table == 0) && (classes_list[i].subclass_of != "") && (classes_list[i].check == "0"))
                    {
                        cout << classes_list[i].name;
                        for (int j = 0; j < clasamount; j++)
                        {
                            if (classes_list[i].subclass_of == classes_safe_list[j].name)
                            {
                                classes_list[j].has_category = 1;
                                
                                if (classes_safe_list[j].is_table == 1)
                                {
                                    classes_list[i].check = "1";
                                    classes_list[i].link = j;
                                }
                                else if (classes_safe_list[j].subclass_of != "")
                                {
                                    classes_list[i].subclass_of = classes_safe_list[j].subclass_of;
                                }
                                else if ((classes_safe_list[j].subclass_of == "") && (classes_safe_list[j].is_table == 0))
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

            // Проверка всех объектных отношений
            while (obj_check > 0)
            {
                obj_check = 0;
                for (int i = 0; i < objamount; i++)
                {
                    if (object_prop_list[i].name != "")
                    {
                        // Определяем имя класса для области определения и области значений

                        for (int j = 0; j < clasamount; j++)
                        {
                            if ((classes_list[j].name == object_prop_list[i].domain) && (classes_list[j].name != "") && (object_prop_list[i].domain != ""))
                            {
                                object_prop_list[i].domain_num = j;
                            }
                            if ((classes_list[j].name == object_prop_list[i].range) && (classes_list[j].name != "") && (object_prop_list[i].range != ""))
                            {
                                object_prop_list[i].range_num = j;
                            }
                        }
                        // Если объектное отношение связывает два класса, имеющие таблицы, объектное отношение 
                        // получает соответствующее свойство "is_table" и ссылку на необходимый класс (domain или range,
                        // в зависимости от наличия свойства reverse_property)
                        if ((classes_list[object_prop_list[i].domain_num].is_table == 1) && (classes_list[object_prop_list[i].range_num].is_table == 1))
                        {
                            object_prop_list[i].check = "1";
                            object_prop_list[i].link = object_prop_list[i].range_num;
                            object_prop_list[i].is_table = 1;
                            if (object_prop_list[i].reverse_property == 1)
                            {
                                object_prop_list[i].link = object_prop_list[i].domain_num;
                            }
                        }
                        // Если только класс области определения является классом с таблицей, объектное отношение наполняется 
                        // литеральными отношениями класса без таблицы, а также получает ссылку на класс таблицы
                        else if ((classes_list[object_prop_list[i].domain_num].is_table == 1) && (classes_list[object_prop_list[i].range_num].is_table == 0))
                        {

                            object_prop_list[i].check = "2";
                            object_prop_list[i].properties = classes_list[object_prop_list[i].range_num].properties;
                            object_prop_list[i].properties_amount = classes_list[object_prop_list[i].range_num].properties_amount;
                            classes_list[object_prop_list[i].range_num].link = object_prop_list[i].domain_num;
                            object_prop_list[i].link = object_prop_list[i].domain_num;

                        }
                        // Если ни один из классов не является таблицей, то область определения данного объектного 
                        // отношения заменяется на ссылку класса изначальной области опредеоения, это будет повторяться
                        // пока область определения не окажется таблицей.
                        else if ((classes_list[object_prop_list[i].domain_num].is_table == 0) && (classes_list[object_prop_list[i].range_num].is_table == 0))
                        {
                            object_prop_list[i].domain = classes_list[classes_list[object_prop_list[i].domain_num].link].name;
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
            // Конец блока обработки сущностей
            //////////////////////////////////////////////////////////////////////////////////////////////////

            /// /////////////////////////////////////////////////////////////////////////////////////////////
            // Начало блока вывода сущностей объявляем и открывем файлы вывода а также необходимые нам переменные
            ofstream tables;
            ofstream documentation;
            documentation.open("documentation.txt");
            tables.open("tables.sql");
            string atribut_change;
            string primar_key;

            cout << "Output start" << endl;
            for (int i = 0; i < clasamount; i++)
            {
                if (classes_list[i].is_table == 1)
                {
                    // Для каждого класса - если он является таблицей для него создаётся новая таблица в файле вывода и 
                    // она заполняется стандартными свойствами (название объекта, id, и так далее), а также свойством категории
                    // если есть
                    tables << "CREATE TABLE " << classes_list[i].clear_name << " (" << endl;
                    cout << endl << "Table " << classes_list[i].clear_name << " create" << endl;
                    tables << "id" << classes_list[i].clear_name << " INT ," << endl;
                    primar_key = "id" + classes_list[i].clear_name;
                    tables << "label TINYTEXT ," << endl;
                    if (classes_list[i].has_category == 1)
                    {
                        tables << category << " TINYTEXT ," << endl;
                    }

                    // Таблицы заполняются свойствами, которые имеют ссылку на эту таблицу и им присваивается тип в соответсвии с 
                    // картой типов, либо же TEXT если нет подходящего типа
                    for (int j = 0; j < datamount; j++)
                    {
                        if (data_prop_list[j].link == i && data_prop_list[j].is_table == 1)
                        {
                            tables << data_prop_list[j].clear_name;
                            if (type_map[data_prop_list[j].range] == "")
                            {
                                tables << " TEXT ," << endl;
                            }
                            else
                            {
                                tables << " " << type_map[data_prop_list[j].range] << endl;
                            }
                        }
                    }

                    for (int j = 0; j < objamount; j++)
                    {
                        if (object_prop_list[j].link == i)
                        {
                            // Если объектное отношение связывает таблицы, то программа создаёт свойство таблицы с id 
                            // связанной с ней
                            if (classes_list[object_prop_list[j].domain_num].is_table == 1 && classes_list[object_prop_list[j].range_num].is_table == 1)
                            {
                                if (object_prop_list[j].reverse_property == 1)
                                {
                                    tables << classes_list[object_prop_list[j].range_num].clear_name + "_id" + classes_list[object_prop_list[j].range_num].clear_name + " INT ," << endl;;
                                }
                                else
                                {
                                    tables << classes_list[object_prop_list[j].domain_num].clear_name + "_id" + classes_list[object_prop_list[j].domain_num].clear_name + " INT ," << endl;
                                }
                            }
                            // В противном случае программа наполняет таблицу отношениями, на которые ссылается 
                            // класс области значений этого объектного отношения 
                            else
                            {
                                for (int m = 1; m <= object_prop_list[j].properties_amount * 2; m = m + 2)
                                {
                                    tables << object_prop_list[j].clear_name + "_";
                                    cout << "Property " << object_prop_list[j].clear_name + "_";
                                    tables << atribut(m, object_prop_list[j].properties);
                                    cout << atribut(m, object_prop_list[j].properties) << " create." << endl;
                                    if (type_map[atribut(2 * m, object_prop_list[j].properties)] != "")
                                    {
                                        tables << " " << type_map[atribut(2 * m, object_prop_list[j].properties)] << endl;
                                    }
                                    else
                                    {
                                        tables << " TEXT ," << endl;
                                    }
                                }
                            }
                        }
                    }
                    tables << "PRIMARY KEY(" + primar_key + ")" << endl;
                    tables << ");" << endl;
                }
            }
            // Вывод документации - список лэйблов и комментариев классов и отношений
            documentation << "Classes: " << endl << endl;
            for (int i = 0; i < clasamount; i++)
            {
                if (classes_list[i].label != "")
                {
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
            // Конец блока вывода
            ///////////////////////////////////////////////////////////////////////////////////////////////////////
            //Завершение программы
            cout << "Output end. Files \"tables.sql\" and \"documentation.txt\" are located in the folder of program." << endl << endl;
            return 0;
        }
    
