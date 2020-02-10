//Jason Eddy N'Guessan | ID: 1079936
//Creds to T.A for helping me to understand how to write c in an object oriented way
//e.g began with initialize objects, andn therein set me off to complete objects
#include "SVGParser_A2.h"
#include <math.h>
#include <ctype.h>

typedef struct
{
    float value;
    char *unit;

} ParsedValue;
ParsedValue *createValue(char *data);
void deleteValue(ParsedValue *data);
char *printFunction(void *data);
void deleteFunction(void *data);
int compareFunction(const void *first, const void *second);
int StartsWith(const char *a, const char *b);
SVGimage *initializeObjects(void);
SVGimage *print_element_names(xmlNode *a_node, SVGimage **list);
Path *createPathObject(char *data);
List *recursiveRect(List *list, Group *group);
List *recursiveCircle(List *list, Group *group);
List *recursivePaths(List *list, Group *group);
List *recursiveGroups(List *list, Group *group);
void insertPath(void *data, xmlNode *cur_node, int version);
void insertRect(void *data, xmlNode *cur_node, int version);
void insertCircle(void *data, xmlNode *cur_node, int version);
void insertGroup(void *data, xmlNode *cur_node, int version);
void insertOtherAttribute(void *data, xmlNode *cur_node);
void insertNSUnits(SVGimage *tempList, xmlNode *cur_node);
Attribute *createAttribute(char *name, char *value);
Circle *createCircleObject(float cx, float cy, float r, char units[50]);
Rectangle *createRectangleObject(float x, float y, float width, float height, char units[50]);
Group *createGroupObject(void);
void validateNameSpace(char *namespace, SVGimage **list);
void validateTitle(char *title, SVGimage **list);
void validateDescription(char *description, SVGimage **list);
int hasAttribute(List *otherAttributes);
//Part 2
bool isAboveZero(char *value);
bool isValidXML(xmlDoc *doc, char *schemaFile);
bool isValidSVGTag(List *tempList);
bool isValidRectTag(List *tempList);
bool isGoodRectangle(Rectangle *rect);
void cleanUp(xmlDoc *doc, SVGimage *list);
bool isGoodRectAttribute(Attribute *attribute, Rectangle *rect);
bool isHalfGreater(Attribute *attribute, Rectangle *rect);
bool isValidPathTag(List *tempList);
bool isProperlySpaced(char *value);
bool isGoodLength(char *value);
bool isValidGroupTag(List *tempList, SVGimage *image);
bool isValidCircleTag(List *tempList);
xmlDocPtr buildTree(SVGimage *image);
void createPath(xmlNodePtr root_element, List *tempList);
void writeAttribute(void *list, xmlNodePtr cur_child);
void createSVG(xmlNodePtr root_element, SVGimage *image);
void createRect(xmlNodePtr root_element, List *tempList);
void createCircle(xmlNodePtr root_element, List *tempList);
void createGroup(xmlNodePtr root_element, List *tempList);
int hasAttribute(List *otherAttributes)
{
    if (otherAttributes->length == 0)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}
char *printFunction(void *data)
{
    return "";
}
void deleteFunction(void *data)
{
}
int compareFunction(const void *first, const void *second)
{
    return 0;
}

int numAttr(SVGimage *img)
{
    int i = 0;
    List *list;
    if (img == NULL)
    {
        return 0;
    }
    else
    {
        void *elem;
        list = getGroups(img);
        ListIterator iter = createIterator(list);
        while ((elem = nextElement(&iter)) != NULL)
        {
            Group *group = (Group *)elem;
            i += group->otherAttributes->length;
        }
        freeList(list);

        list = getRects(img);
        iter = createIterator(list);
        while ((elem = nextElement(&iter)) != NULL)
        {
            Rectangle *rect = (Rectangle *)elem;
            i += rect->otherAttributes->length;
        }
        freeList(list);

        list = getCircles(img);

        iter = createIterator(list);
        while ((elem = nextElement(&iter)) != NULL)
        {
            Circle *circle = (Circle *)elem;
            i += circle->otherAttributes->length;
        }
        freeList(list);

        list = getPaths(img);

        iter = createIterator(list);
        while ((elem = nextElement(&iter)) != NULL)
        {
            Path *path = (Path *)elem;
            i += path->otherAttributes->length;
        }
        freeList(list);

        i += img->otherAttributes->length;
    }
    // printf("%d", getLength(img -> otherAttributes));
    //printf("Attrb %d", i);
    return i;
}
//gets num of groups with length
int numGroupsWithLen(SVGimage *img, int len)
{
    if (img == NULL || getLength(img->groups) == 0)
    {
        return 0;
    }
    int counter = 0;
    void *elem;
    List *list = getGroups(img);

    ListIterator iter = createIterator(list);
    while ((elem = nextElement(&iter)) != NULL)
    {
        int i = 0;
        Group *group = (Group *)elem;
        i += group->circles->length;
        i += group->rectangles->length;
        i += group->paths->length;
        i += group->groups->length;
        i += group->otherAttributes->length;

        if (i == len)
        {
            counter++;
        }
    }
    freeList(list);

    return counter;
}

int numRectsWithArea(SVGimage *img, float area)
{
    int num = 0;
    if (img == NULL || img->rectangles->length == 0)
    {
        return 0;
    }
    else
    {
        void *elem;
        List *list;
        list = getRects(img);
        ListIterator iter = createIterator(list);
        while ((elem = nextElement(&iter)) != NULL)
        {
            Rectangle *rectangle = (Rectangle *)elem;
            float calc = rectangle->height * rectangle->width;
            if (ceil(calc) == ceil(area))
            {
                num++;
            }
        }
        freeList(list);
    }
    return num;
}
int numCirclesWithArea(SVGimage *img, float area)
{
    int num = 0;
    if (img == NULL || img->circles->length == 0)
    {
        return 0;
    }
    else
    {
        List *list;
        list = getCircles(img);
        void *elem;
        ListIterator iter = createIterator(list);
        while ((elem = nextElement(&iter)) != NULL)
        {
            Circle *circle = (Circle *)elem;
            float calc = 3.14159265358979323846 * (circle->r * circle->r);
            if (ceil(calc) == ceil(area))
            {
                num++;
            }
        }
        freeList(list);
    }
    return num;
}
int numPathsWithdata(SVGimage *img, char *data)
{

    char *str;
    List *list;

    int num = 0;
    if (img == NULL || img->paths->length == 0 || data == NULL)
    {
        //  printf("null");
        return 0;
    }
    else
    {
        str = malloc(1000);

        void *elem;
        list = getPaths(img);

        ListIterator iter = createIterator(list);
        while ((elem = nextElement(&iter)) != NULL)
        {

            Path *path = (Path *)elem;
            printf("\n%s\n", path->data);
            strcpy(str, path->data);
            if (strcmp(str, data) == 0)
            {
                num++;
            }
        }
        free(str);
        freeList(list);
    }

    return num;
}

char *SVGimageToString(SVGimage *img)
{

    /*
    List* circleList = img -> circles;
    List* rectangleList = img -> rectangles;
    List* pathList = img -> paths;
    List* groupList = img -> groups;
    List* otherList = img ->otherAttributes;
    printf("Namespace: %s\n", img -> namespace  );
    printf("Title: %s\n", img -> title  );
    printf("Description: %s\n", img -> description  );
    printf("\nCircle: \n");
    char *circle = NULL;
    char *rectangle = NULL;
    char *toStrings = toString(img -> circles);
    circle = malloc(strlen(toStrings) + 2);
    strcpy(circle, toStrings );
    printf("%s", circle);
    free(circle);
    free(toStrings);
    rectangle = malloc(strlen(toStrings) + 2);
    toStrings = toString(img -> rectangles);
    strcpy(rectangle, toStrings);
    printf("%s", rectangle);
     */

    //freeing the value
    //freeing the memory
    /*
    char *value;
    while(rectangleList -> head != NULL){
      value = rectangleToString((Circle *)circleList -> head -> data);
      printf("%s", value);
      circleList -> head = circleList -> head -> next;
          free(value);
          free(circleList -> head);
      }
     */

    char *temp = malloc(10);
    if (img == NULL)
    {
        strcpy(temp, " ");

        return temp;
    }

    return temp;
}

List *getGroups(SVGimage *img)
{
    if (img == NULL)
    {
        return 0;
    }

    List *list = initializeList(printFunction, deleteFunction, compareFunction);
    ListIterator iter2 = createIterator(img->groups);
    void *elem2;
    while ((elem2 = nextElement(&iter2)) != NULL)
    {
        Group *group = (Group *)elem2;
      
        insertBack(list, group);
        recursiveGroups(list, group);
    }

    return list;
}

List *recursiveGroups(List *list, Group *group)
{

    void *elem3;

    if (group->groups != NULL)
    {

        ListIterator iter3 = createIterator(group->groups);
        //Our incrementer
        while ((elem3 = nextElement(&iter3)) != NULL)
        {
            Group *grp = (Group *)elem3;
            //Calls function & restarts

           
            insertBack(list, grp);

            list = recursiveGroups(list, grp);
        }
    }

    return list;
}

List *getPaths(SVGimage *img)
{

    if (img == NULL)
    {
        return 0;
    }
    List *list = initializeList(printFunction, deleteFunction, compareFunction);
    ListIterator iter = createIterator(img->paths);
    ListIterator iter2 = createIterator(img->groups);

    void *elem;
    void *elem2;

    //outside
    while ((elem = nextElement(&iter)) != NULL)
    {
        Path *circle = (Path *)elem;
        insertBack(list, circle);
    }
    //inner group

    while ((elem2 = nextElement(&iter2)) != NULL)
    {
        Group *group = (Group *)elem2;

        //looks for groups that have rectangles
        recursivePaths(list, group);
    }

    // printf("\nGetCircle length: %d\n", getLength(list));
    return list;
}

List *recursivePaths(List *list, Group *group)
{
    void *elem3;

    if (group->paths != NULL)
    {

        ListIterator iter3 = createIterator(group->paths);

        while ((elem3 = nextElement(&iter3)) != NULL)
        {
            Path *rect = (Path *)elem3;
            insertBack(list, rect);
        }
    }

    if (group->groups != NULL)
    {

        ListIterator iter3 = createIterator(group->groups);
        //Our incrementer
        while ((elem3 = nextElement(&iter3)) != NULL)
        {
            Group *grp = (Group *)elem3;
            //Calls function & restarts
            list = recursivePaths(list, grp);
        }
    }

    return list;
}

List *getCircles(SVGimage *img)
{

    if (img == NULL)
    {
        return 0;
    }
    List *list = initializeList(printFunction, deleteFunction, compareFunction);
    ListIterator iter = createIterator(img->circles);
    ListIterator iter2 = createIterator(img->groups);

    void *elem;
    void *elem2;

    //outside
    while ((elem = nextElement(&iter)) != NULL)
    {
        Circle *circle = (Circle *)elem;
        insertBack(list, circle);
    }
    //inner group

    while ((elem2 = nextElement(&iter2)) != NULL)
    {
        Group *group = (Group *)elem2;

        //looks for groups that have rectangles
        recursiveCircle(list, group);
    }

    // printf("\nGetCircle length: %d\n", getLength(list));
    return list;
}

List *recursiveCircle(List *list, Group *group)
{
    void *elem3;

    if (group->rectangles != NULL)
    {

        ListIterator iter3 = createIterator(group->circles);

        while ((elem3 = nextElement(&iter3)) != NULL)
        {
            Circle *rect = (Circle *)elem3;
            insertBack(list, rect);
        }
    }

    if (group->groups != NULL)
    {

        ListIterator iter3 = createIterator(group->groups);
        //Our incrementer
        while ((elem3 = nextElement(&iter3)) != NULL)
        {
            Group *grp = (Group *)elem3;
            //Calls function & restarts
            list = recursiveCircle(list, grp);
        }
    }

    return list;
}

List *getRects(SVGimage *img)
{
    if (img == NULL)
    {
        return 0;
    }
    List *list = initializeList(printFunction, deleteFunction, compareFunction);
    ListIterator iter = createIterator(img->rectangles);
    ListIterator iter2 = createIterator(img->groups);

    void *elem;
    void *elem2;

    //outside
    while ((elem = nextElement(&iter)) != NULL)
    {
        Rectangle *rect = (Rectangle *)elem;
        insertBack(list, rect);
    }
    //inner group

    while ((elem2 = nextElement(&iter2)) != NULL)
    {
        Group *group = (Group *)elem2;
        //looks for groups that have rectangles
        recursiveRect(list, group);
    }
    return list;
}

List *recursiveRect(List *list, Group *group)
{
    void *elem3;

    if (group->rectangles != NULL)
    {

        ListIterator iter3 = createIterator(group->rectangles);

        while ((elem3 = nextElement(&iter3)) != NULL)
        {
            Rectangle *rect = (Rectangle *)elem3;
            insertBack(list, rect);
            //  printf("hi\t");
        }
    }

    if (group->groups != NULL)
    {

        ListIterator iter3 = createIterator(group->groups);
        //Our incrementer
        while ((elem3 = nextElement(&iter3)) != NULL)
        {
            Group *grp = (Group *)elem3;
            //Calls function & restarts
            list = recursiveRect(list, grp);
        }
    }
    //Base case
    return list;
}

SVGimage *createSVGimage(char *fileName)
{

    //Initialize Our tempData - we'll be reusing this memory, modifying the data
    xmlDoc *doc = NULL;
    xmlNode *root_element = NULL;
    SVGimage *list = initializeObjects();

    LIBXML_TEST_VERSION
    doc = xmlReadFile(fileName, NULL, 0);
    if (doc == NULL)
    {
        //   printf("error: could not parse file %s\n", fileName);
        return NULL;
    }
    else
    {

        /*Get the root element node */
        root_element = xmlDocGetRootElement(doc);
        print_element_names(root_element, &list);
        validateNameSpace((char *)root_element->ns->href, &list);
    }

    xmlFreeDoc(doc);
    xmlCleanupParser();
    return list;

    //Returns the pointer of type SVGimage containing all data
}

//Got this from prof
SVGimage *createValidSVGimage(char *fileName, char *schemaFile)
{

    //Initialize Our tempData - we'll be reusing this memory, modifying the data
    xmlDoc *doc = NULL;
    xmlNode *root_element = NULL;
    SVGimage *list = initializeObjects();

    LIBXML_TEST_VERSION
    doc = xmlReadFile(fileName, NULL, 0);
    if (doc == NULL)
    {
        printf("error: could not parse file %s\n", fileName);
        return NULL;
    }
    else
    {
        //Checks if it's a valid XML
        if (isValidXML(doc, schemaFile) == true)
        {
            /*Get the root element node */
            root_element = xmlDocGetRootElement(doc);
            print_element_names(root_element, &list);
            validateNameSpace((char *)root_element->ns->href, &list);

            //VALIDATING SVG
            if (validateSVGimage(list, schemaFile) == true)
            {
                writeSVGimage(list, fileName);
            }
            else
            {
                //JUST ADDED
                cleanUp(doc, list);
                printf("\nNot a valid SVG Image\n");
                return NULL;
            }
        }
        else
        {

            cleanUp(doc, list);
            printf("\nNot a valid XML File\n");
            return NULL;
        }
    }

    xmlFreeDoc(doc);
    xmlCleanupParser();
    return list;

    //Returns the pointer of type SVGimage containing all data
}

/*
 
 */

//make sure to validate for lengths are too long
//TEST FOR EMPTY TAGS
//do we need to validate the write?
//XMLDocPtr for writing docs
xmlDocPtr buildTree(SVGimage *image)
{
    xmlDocPtr doc = NULL;
    xmlNodePtr root_element = NULL;
    xmlNsPtr nameSpace;
    doc = xmlNewDoc(BAD_CAST "1.0");
    //creates a pointer to new root_node
    root_element = xmlNewNode(NULL, BAD_CAST "svg");
    nameSpace = xmlNewNs(root_element, BAD_CAST image->namespace, NULL);
    xmlSetNs(root_element, nameSpace);
    xmlDocSetRootElement(doc, root_element);
    createSVG(root_element, image);
    createRect(root_element, image->rectangles);
    createCircle(root_element, image->circles);
    createPath(root_element, image->paths);
    List *_getGroups = getGroups(image);
    createGroup(root_element, _getGroups);
    freeList(_getGroups);

    // xmlNewChild(root_element, BAD_CAST image -> title);
    // xmlNewProp(root_element, BAD_CAST "title", BAD_CAST image -> title);
    return doc;
}
void createGroup(xmlNodePtr root_element, List *tempList)
{

    ListIterator iter2 = createIterator(tempList);
    void *elem;
    while ((elem = nextElement(&iter2)) != NULL)
    {
        Group *group = (Group *)elem;
        xmlNodePtr cur_child = xmlNewChild(root_element, NULL, BAD_CAST "g", BAD_CAST "HH");
                                    
        if (group -> otherAttributes != NULL)
        {
            writeAttribute(group->otherAttributes, cur_child);
        }
    }
}
void writeAttribute(void *list, xmlNodePtr cur_child)
{
    void *elem;

    List *tempList = (void *)list;
    ListIterator iter = createIterator(tempList);

    while ((elem = nextElement(&iter)) != NULL)
    {

        Attribute *attribute = (Attribute *)elem;
                printf("%s\t", attribute ->value);
        xmlNewProp(cur_child, BAD_CAST attribute->name, BAD_CAST attribute->value);
    
    }
    printf("\n");

}

void createRect(xmlNodePtr root_element, List *tempList)
{

    void *elem;
    char toFloat[100];
    ListIterator iter = createIterator(tempList);

    while ((elem = nextElement(&iter)) != NULL)
    {
        Rectangle *rect = (Rectangle *)elem;
        if (rect != NULL)
        {
            xmlNodePtr cur_child = xmlNewChild(root_element, NULL, BAD_CAST "rect", BAD_CAST "");
            if (strlen(rect->units) > 1)
            {
                //RETURNS AN XML NODE PTR

                //two decimal points

                snprintf(toFloat, sizeof(toFloat), "%.1f", rect->x);
                xmlNewProp(cur_child, BAD_CAST "x", BAD_CAST strcat(toFloat, rect->units));

                snprintf(toFloat, sizeof(toFloat), "%.1f", rect->y);
                xmlNewProp(cur_child, BAD_CAST "y", BAD_CAST strcat(toFloat, rect->units));

                snprintf(toFloat, sizeof(toFloat), "%.1f", rect->width);
                xmlNewProp(cur_child, BAD_CAST "width", BAD_CAST strcat(toFloat, rect->units));

                snprintf(toFloat, sizeof(toFloat), "%.1f", rect->height);
                xmlNewProp(cur_child, BAD_CAST "height", BAD_CAST strcat(toFloat, rect->units));
            }
            else
            {
                snprintf(toFloat, sizeof(toFloat), "%.1f", rect->x);
                xmlNewProp(cur_child, BAD_CAST "x", BAD_CAST toFloat);

                snprintf(toFloat, sizeof(toFloat), "%.1f", rect->y);
                xmlNewProp(cur_child, BAD_CAST "y", BAD_CAST toFloat);

                snprintf(toFloat, sizeof(toFloat), "%.1f", rect->width);
                xmlNewProp(cur_child, BAD_CAST "width", BAD_CAST toFloat);

                snprintf(toFloat, sizeof(toFloat), "%.1f", rect->height);
                xmlNewProp(cur_child, BAD_CAST "height", BAD_CAST toFloat);
            }

            if (rect->otherAttributes != NULL)
            {
                writeAttribute(rect->otherAttributes, cur_child);
            }
        }
    }
}
void createCircle(xmlNodePtr root_element, List *tempList)
{

    void *elem;
    char toFloat[100];
    ListIterator iter = createIterator(tempList);

    while ((elem = nextElement(&iter)) != NULL)
    {
        Circle *circle = (Circle *)elem;
        if (circle != NULL)
        {
            xmlNodePtr cur_child = xmlNewChild(root_element, NULL, BAD_CAST "circle", BAD_CAST "");
            if (strlen(circle->units) > 1)
            {
                //RETURNS AN XML NODE PTR

                //two decimal points

                snprintf(toFloat, sizeof(toFloat), "%.1f", circle->cx);
                xmlNewProp(cur_child, BAD_CAST "cx", BAD_CAST strcat(toFloat, circle->units));

                snprintf(toFloat, sizeof(toFloat), "%.1f", circle->cy);
                xmlNewProp(cur_child, BAD_CAST "cy", BAD_CAST strcat(toFloat, circle->units));

                snprintf(toFloat, sizeof(toFloat), "%.1f", circle->r);
                xmlNewProp(cur_child, BAD_CAST "r", BAD_CAST strcat(toFloat, circle->units));
            }
            else
            {
                snprintf(toFloat, sizeof(toFloat), "%.1f", circle->cx);
                xmlNewProp(cur_child, BAD_CAST "cx", BAD_CAST toFloat);

                snprintf(toFloat, sizeof(toFloat), "%.1f", circle->cy);
                xmlNewProp(cur_child, BAD_CAST "cy", BAD_CAST toFloat);

                snprintf(toFloat, sizeof(toFloat), "%.1f", circle->r);
                xmlNewProp(cur_child, BAD_CAST "r", BAD_CAST toFloat);
            }

            if (circle->otherAttributes != NULL)
            {
                writeAttribute(circle->otherAttributes, cur_child);
            }
        }
    }
}
void createPath(xmlNodePtr root_element, List *tempList)
{

    void *elem;
    ListIterator iter = createIterator(tempList);

    while ((elem = nextElement(&iter)) != NULL)
    {
        Path *path = (Path *)elem;
        if (path->data != NULL)
        {
            //RETURNS AN XML NODE PTR

            xmlNodePtr cur_child = xmlNewChild(root_element, NULL, BAD_CAST "path", BAD_CAST "");
            xmlNewProp(cur_child, BAD_CAST "d", BAD_CAST path->data);

            if (path->otherAttributes != NULL)
            {
                writeAttribute(path->otherAttributes, cur_child);
            }
        }
    }
}

void createSVG(xmlNodePtr root_element, SVGimage *image)
{

    //e.g title
    xmlNewChild(root_element, NULL, BAD_CAST "title", BAD_CAST image->title);
    //description
    xmlNewChild(root_element, NULL, BAD_CAST "desc", BAD_CAST image->description);

    writeAttribute(image->otherAttributes, root_element);
}

//create function for circle, rect, etc
bool writeSVGimage(SVGimage *image, char *fileName)
{
    //Create an xml file: validSVG == true ? return true : return false
    bool valid = true;
    if (image == NULL || image->circles == NULL || image->rectangles == NULL || image->paths == NULL || image->groups == NULL || image->otherAttributes == NULL || fileName == NULL)
    {
        return false;
    }

    /*
               //searches for last occurence of period
               char *extension = strrchr(fileName, '.');
               if(extension != NULL){
                   if(strcmp(extension + 1, "svg") == 0){
                       printf("FOUND!!!");
                   }
               }
                return false;
                */

    xmlDocPtr tree = buildTree(image);

    int result = xmlSaveFormatFileEnc("my.svg", tree, "UTF-8", 1);
    xmlFreeDoc(tree);
    if (result < 0)
    {
        return false;
    }

    return true;
}
//Cleans up failed attemp to create an SVG image
void cleanUp(xmlDoc *doc, SVGimage *list)
{
    xmlFreeDoc(doc);
    xmlCleanupParser();
    deleteSVGimage(list);
}

bool validateSVGimage(SVGimage *doc, char *schemaFile)
{
    SVGimage *image = doc;
    if (image == NULL || image->circles == NULL || image->rectangles == NULL || image->paths == NULL || image->groups == NULL || image->otherAttributes == NULL)
    {
        printf("no good");
        return false;
    }

    else
    {
        bool valid = true;
        valid = isValidSVGTag(image->otherAttributes);
        if (valid == false)
        {
            printf("invalid @ svg");
            return valid;
        }
        valid = isValidPathTag(image->paths);
        if (valid == false)
        {
            printf("invalid @ Path");

            return valid;
        }
        valid = isValidCircleTag(image->circles);
        if (valid == false)
        {
            printf("invalid @ Circle");

            return valid;
        }
        valid = isValidRectTag(image->rectangles);
        if (valid == false)
        {
            printf("invalid @ Rect");
            return valid;
        }
        valid = isValidGroupTag(image->groups, image);
        if (valid == false)
        {
            printf("invalid @ Group");
            return valid;
        }

        return true;
    }
}
/*
 bool isValidPathTag(List *tempList){
     bool valid = true;
     void *elem;
     ListIterator iter = createIterator(tempList);
     
     while ((elem = nextElement(&iter)) != NULL)
     {
         Path *path = (Path *)elem;
        valid = isProperlySpaced(path -> data);
         if(valid == false){
             return valid;
             break;
         }
         
         
     }
     return valid;
 }
 */

bool isValidGroupTag(List *tempList, SVGimage *image)
{
    bool valid = true;

    List *list = getPaths(image);
    valid = isValidPathTag(list);
    freeList(list);
    if (valid == false)
    {
        return false;
    }
    list = getCircles(image);
    valid = isValidCircleTag(list);
    freeList(list);
    if (valid == false)
    {
        return false;
    }
    list = getRects(image);
    valid = isValidRectTag(list);
    freeList(list);
    if (valid == false)
    {
        return false;
    }
    return valid;
}

bool isValidRectTag(List *tempList)
{
    bool valid = true;
    void *elem;
    ListIterator iter = createIterator(tempList);
    while ((elem = nextElement(&iter)) != NULL)
    {
        Rectangle *rect = (Rectangle *)elem;
        valid = isGoodRectangle(rect);
    }
    return valid;
}
bool isGoodRectangle(Rectangle *rect)
{

    if (rect->width < 0 || rect->height < 0)
    {
        return false;
    }
    if (rect->otherAttributes == NULL)
    {
        return false;
    }
    else
    {
        bool valid = true;
        void *elem;
        List *tempList = rect->otherAttributes;
        ListIterator iter = createIterator(tempList);
        while ((elem = nextElement(&iter)) != NULL)
        {
            Attribute *attribute = (Attribute *)elem;
            valid = isGoodRectAttribute(attribute, rect);
            if (valid == false)
            {
                return false;
                break;
            }
        }
    }
    return true;
}

bool isGoodRectAttribute(Attribute *attribute, Rectangle *rect)
{
    bool valid = true;
    if (strcmp(attribute->name, "rx") == 0 || strcmp(attribute->name, "ry") == 0)
    {
        valid = isAboveZero(attribute->value);
        if (valid == false)
        {
            return false;
        }
        valid = isHalfGreater(attribute, rect);
        if (valid == false)
        {
            return false;
        }
    }
    return true;
}
/*
 W3C schools
 If rx is greater than half of ‘width’, then set rx to half of ‘width’.
 If ry is greater than half of ‘height’, then set ry to half of ‘height’.
 */
bool isHalfGreater(Attribute *attribute, Rectangle *rect)
{
    bool valid = true;
    ParsedValue *rxry = NULL;
    rxry = createValue(attribute->value);
    if (strcmp(attribute->name, "rx") == 0)
    {
        if ((rxry->value) > (rect->width / 2))
        {
            valid = false;
        }
    }
    //then it has to be ry
    else
    {
        if ((rxry->value) > (rect->height / 2))
        {
            valid = false;
        }
    }
    deleteValue(rxry);

    return valid;
}
bool isValidCircleTag(List *tempList)
{
    bool valid = true;
    void *elem;
    ListIterator iter = createIterator(tempList);
    while ((elem = nextElement(&iter)) != NULL)
    {
        Circle *circle = (Circle *)elem;
        if (circle->r < 0)
        {
            return false;
            break;
        }
    }

    return valid;
}

bool isAboveZero(char *value)
{
    int i;
    char array[strlen(value)];
    strcpy(array, value);
    int unit = 0;
    for (i = 0; i < strlen(value); i++)
    {
        //Sees if there is a negative
        if (array[i] == '-')
        {
            return false;
            break;
        }

        //if there's no space, then it better be a type and it's not a digit and never been entered
        if (isdigit(array[i]) == false && array[i] != ' ' && array[i] != '.' && array[i] != ',' && unit == 0)
        {
            unit++;
            bool acceptableLength = isGoodLength(value);
            if (acceptableLength == false)
            {
                return false;
                break;
            }
        }
    }

    return true;
}

//only use when the value has been throghoughly iterated
bool isGoodLength(char *value)
{
    if (strstr(value, "em") != NULL || strstr(value, "ex") != NULL ||
        strstr(value, "px") != NULL || strstr(value, "in") != NULL || strstr(value, "cm") != NULL || strstr(value, "mm") != NULL || strstr(value, "pt") != NULL ||
        strstr(value, "%") != NULL ||
        strstr(value, "pc") != NULL

    )
    {
        return true;
    }
    else
    {
        printf("invalid Length\n");
        return false;
    }
}

bool isValidPathTag(List *tempList)
{
    bool valid = true;
    void *elem;
    ListIterator iter = createIterator(tempList);

    while ((elem = nextElement(&iter)) != NULL)
    {
        Path *path = (Path *)elem;
        valid = isProperlySpaced(path->data);
        if (valid == false)
        {
            return valid;
        }
    }
    return valid;
}
//Checks if path is probably spaced
bool isProperlySpaced(char *value)
{

    int i;
    char array[strlen(value)];
    strcpy(array, value);
    int digit = 1;
    int asci;
    for (i = 0; i < strlen(value); i++)
    {
        if (isdigit(array[i]) == false)
        {
            asci = array[i];
            if (asci >= 65 && asci <= 122)
            {
                //Z means end of path
                if (array[i] == 'z' && i == strlen(value) - 1)
                {
                    break;
                }
                //go to next int
                i++;
                while (true)
                {
                    asci = array[i];
                    if (i == strlen(value))
                    {
                        break;
                    }
                    //checks if the next asci is a space

                    int nextAsci = array[i + 1];
                    //comma or dash (-)

                    if (asci == 44 || asci == 45)
                    {
                        digit++;
                    }
                    //if it's a space
                    if (nextAsci == 32)
                    {

                        //check untill the next Alphabetical
                        nextAsci = array[i + 2];
                        if (nextAsci >= 65 && nextAsci <= 122 && array[i + 3] != '-')
                        {
                            //end of path data

                            break;
                        }
                        //If is a space, not alphabetic, and the next two is a digit - increment

                        nextAsci = array[i + 1];

                        if (nextAsci == 32 && isdigit(array[i + 2]))
                        {

                            digit++;
                        }
                    }
                    //increment array inside;
                    i++;
                }

                //printf("\nRESETTED %d @ I %d && max is %d\n", digit, i, strlen(value));

                if (digit % 2 != 0)
                {
                    return false;
                }
                digit = 1;
            }
        }
    }

    printf("\nComplete cycle %d\n", digit);
    return true;
}

bool isValidSVGTag(List *tempList)
{
    bool valid = true;
    void *elem;
    ListIterator iter = createIterator(tempList);

    while ((elem = nextElement(&iter)) != NULL)
    {
        Attribute *svg = (Attribute *)elem;
        if (strcmp("version", svg->name) == 0)
        {
            valid = isAboveZero(svg->value);
            if (!valid)
            {
                return false;
                break;
            }
        }
        if (strcmp("x", svg->name) == 0)
        {
            valid = isAboveZero(svg->value);
            if (!valid)
            {
                return false;
                break;
            }
        }

        if (strcmp("y", svg->name) == 0)
        {
            valid = isAboveZero(svg->value);
            if (!valid)
            {
                return false;
                break;
            }
        }
        if (strcmp("height", svg->name) == 0)
        {
            valid = isAboveZero(svg->value);
            if (!valid)
            {
                return false;
                break;
            }
        }
        if (strcmp("width", svg->name) == 0)
        {
            valid = isAboveZero(svg->value);
            if (!valid)
            {

                return false;
                break;
            }
        }
        if (strcmp("viewBox", svg->name) == 0)
        {

            valid = isAboveZero(svg->value);
            if (!valid)
            {
                return false;
                break;
            }
        }
    }
    return valid;
}

//http://knol2share.blogspot.com/2009/05/validate-xml-against-xsd-in-c.html
//Link given by prof
bool isValidXML(xmlDoc *doc, char *schemaFile)
{

    xmlSchemaPtr schema = NULL;
    xmlSchemaParserCtxtPtr xmlContxt;
    xmlLineNumbersDefault(1);

    //Creates an XML Schemas parse context
    xmlContxt = xmlSchemaNewParserCtxt(schemaFile);

    //Handles errors within the parser context (returns 0 if good)
    xmlSchemaSetParserErrors(xmlContxt, (xmlSchemaValidityErrorFunc)fprintf, (xmlSchemaValidityWarningFunc)fprintf, stderr);
    //returns an xml Structure for me to comapre with
    schema = xmlSchemaParse(xmlContxt);
    xmlSchemaFreeParserCtxt(xmlContxt);
    int value = -1;
    xmlSchemaValidCtxtPtr validator;
    validator = xmlSchemaNewValidCtxt(schema);
    xmlSchemaSetValidErrors(validator, (xmlSchemaValidityErrorFunc)fprintf, (xmlSchemaValidityWarningFunc)fprintf, stderr);

    value = xmlSchemaValidateDoc(validator, doc);
    if (value == 0)
    {
        if (schema != NULL)
        {
            xmlSchemaFree(schema);
        }
        xmlSchemaFreeValidCtxt(validator);
        xmlSchemaCleanupTypes();
        xmlMemoryDump();
        return true;
    }
    else
    {

        if (schema != NULL)
        {
            xmlSchemaFree(schema);
        }
        xmlSchemaFreeValidCtxt(validator);
        xmlSchemaCleanupTypes();
        xmlMemoryDump();
        return false;
    }
}

SVGimage *print_element_names(xmlNode *a_node, SVGimage **list)
{
    SVGimage *tempList = *list;
    xmlNode *cur_node = NULL;
    int i = 0;

    //Iterates through xmlNode given by root_element, as you can see by its name
    for (cur_node = a_node; cur_node != NULL; cur_node = cur_node->next)
    {

        char *parent = (char *)cur_node->parent->name;

        i++;
        if (cur_node->type == XML_ELEMENT_NODE)
        {

            // printf("i: %d node type: Element, name: %s\n", i, cur_node->name);
            if (strcmp((char *)cur_node->name, "title") == 0)
            {

                validateTitle((char *)cur_node->children->content, &tempList);
            }
            else if (strcmp((char *)cur_node->name, "desc") == 0)
            {
                validateDescription((char *)cur_node->children->content, &tempList);
            }
        }
        if (strcmp((char *)cur_node->name, "svg") == 0)
        {
            insertOtherAttribute(tempList, cur_node);
        }

        if (cur_node->content != NULL)
        {
            // =printf("i:%d content: %s \n", i, cur_node -> content);
        }

        if (parent != NULL)
        {
            if (strcmp(parent, "g") != 0)
            {

                if (strcmp((char *)cur_node->name, "circle") == 0)
                {

                    insertCircle(tempList, cur_node, 0);
                }
                if (strcmp((char *)cur_node->name, "path") == 0)
                {

                    insertPath(tempList, cur_node, 0);
                }
                if (strcmp((char *)cur_node->name, "rect") == 0)
                {

                    insertRect(tempList, cur_node, 0);
                }
                if (strcmp((char *)cur_node->name, "g") == 0)
                {

                    insertGroup(tempList, cur_node, 0);
                }
                /*
            if(strcmp((char *)cur_node -> name, "g") != 0 && strcmp((char *)cur_node -> name, "circle") != 0 && strcmp((char *)cur_node -> name, "path") != 0 && strcmp((char *)cur_node -> name, "rect") != 0  && strcmp((char *)cur_node -> name, "text") != 0  && strcmp((char *)cur_node -> name, "comment") != 0 && strcmp((char *)cur_node -> name, "desc") != 0 && strcmp((char *)cur_node -> name, "title") != 0     ){
                insertOtherAttribute(tempList, cur_node);               // printf("%s", attribute ->value);
            }
             */
                /*Uncomment the code below if you want to see the content of every node.
        
        */

                //increments by children
            }
            else
            {

                //G
            }
        }
        else
        {
        }
        print_element_names(cur_node->children, &tempList);
    }

    return tempList;
}

void insertOtherAttribute(void *data, xmlNode *cur_node)
{
    SVGimage *list = data;

    xmlAttr *attr;

    for (attr = cur_node->properties; attr != NULL; attr = attr->next)
    {
        xmlNode *snapshot = attr->children;
        char *getAttrValue = (char *)snapshot->content;
        char *getAttrName = (char *)attr->name;

        if (getAttrValue != NULL && getAttrName != NULL)
        {

            Attribute *attribute = createAttribute(getAttrName, getAttrValue);
            //printf("attrn: %s", getAttrName);
            insertBack(list->otherAttributes, attribute);
        }
    }
}
/*
void insertOtherAttribute(void *data, xmlNode *cur_node){
    SVGimage *list = data;

    xmlAttr *attr;
           // printf("%d\n",i);
    
    for (attr = cur_node->properties; attr != NULL; attr = attr->next)
    {
        xmlNode *snapshot = attr->children;
        char *getAttrValue = (char *)snapshot->content;
        char *getAttrName = (char *)attr->name;
        char *name = calloc(1, strlen(getAttrName) + strlen((char *)cur_node -> name)  + 10);
        strcat(name, (char *)cur_node -> name );
        strcat(name, " >");
        strcat(name, getAttrName);
        Attribute *attribute = createAttribute(name, getAttrValue);
        insertBack(list -> otherAttributes, attribute);
      
        free(name);
    }
 }
 */

void insertGroup(void *data, xmlNode *cur_node, int version)
{
    int i = 0;
    //printf("%s");
    xmlAttr *attr;
    Group *group = NULL;
    if (i == 0)
    {
        group = createGroupObject();
    }

    attr = cur_node->properties;
    // printf("%d\n",i);

    xmlNode *snapshot = attr->children;
    char *getAttrValue = (char *)snapshot->content;
    char *getAttrName = (char *)attr->name;
    if (getAttrValue != NULL && getAttrName != NULL)
    {
        for (attr = cur_node->properties; attr != NULL; attr = attr->next)
        {
            if (version == 0)
            {

                Attribute *attribute = createAttribute(getAttrName, getAttrValue);
                insertBack(group->otherAttributes, attribute);

            }
            if (version == 1)
            {
                Attribute *attribute = createAttribute(getAttrName, getAttrValue);

                insertBack(group->otherAttributes, attribute);
            }
        }

        //9
        long nodeCounter = xmlChildElementCount(cur_node);
        xmlNode *temp_cur_children = cur_node->children;
        xmlNode *temp_cur_node;
        //Looks at the sibling of the current children
        // printf("\n%ld\n", nodeCounter);
        const char *validateName;

        while (nodeCounter != 0)
        {

            temp_cur_node = xmlNextElementSibling(temp_cur_children);
            if (temp_cur_node != NULL)
            {
                validateName = (const char *)temp_cur_node->name;
                // printf("%s\n", validateName);
                if (strcmp(validateName, "rect") == 0)
                {

                    insertRect(group, temp_cur_node, 1);
                    //we'll be storing the other attributes here as well;xxw
                }

                if (strcmp(validateName, "circle") == 0)
                {
                    insertCircle(group, temp_cur_node, 1);
                }
                if (strcmp(validateName, "path") == 0)
                {
                    insertPath(group, temp_cur_node, 1);
                }
                if (strcmp(validateName, "g") == 0)
                {
                    //Calls recursion - goes to the nexr group
                    insertGroup(group, temp_cur_node, 1);
                }
            }

            temp_cur_children = temp_cur_children->next->next;

            nodeCounter--;
        }
        if (nodeCounter == 0)
        {
            if (version == 0)
            {
                SVGimage *list = data;
                insertBack(list->groups, group);
            }
            if (version == 1)
            {
                Group *list = data;
                insertBack(list->groups, group);
            }
        }
    }
}

///Version is so that we could reuse this with different types of list
void insertRect(void *data, xmlNode *cur_node, int version)
{
    int i = 0;
    //printf("%s");
    xmlAttr *attr;
    float x = 0, y = 0, width = 0, height = 0;
    ParsedValue *parsedValue = NULL;

    for (attr = cur_node->properties; attr != NULL; attr = attr->next)
    {
        i++;
        // printf("%d\n",i);
        xmlNode *snapshot = attr->children;
        char *getAttrValue = (char *)snapshot->content;
        char *getAttrName = (char *)attr->name;

        if (getAttrValue != NULL && getAttrName != NULL)
        {

            //seperate them
            if (strcmp("x", getAttrName) == 0)
            {

                if (parsedValue == NULL)
                {
                    parsedValue = createValue(getAttrValue);
                    x = parsedValue->value;
                }
                else
                {
                    x = atof(getAttrValue);
                }
            }
            if (strcmp("y", getAttrName) == 0)
            {
                if (parsedValue == NULL)
                {
                    parsedValue = createValue(getAttrValue);
                    y = parsedValue->value;
                }
                else
                {
                    y = atof(getAttrValue);
                }
            }
            if (strcmp("width", getAttrName) == 0)
            {

                if (parsedValue == NULL)
                {
                    parsedValue = createValue(getAttrValue);
                    width = parsedValue->value;
                }
                else
                {
                    width = atof(getAttrValue);
                }
            }
            if (strcmp("height", getAttrName) == 0)
            {
                if (parsedValue == NULL)
                {
                    parsedValue = createValue(getAttrValue);
                    height = parsedValue->value;
                }
                else
                {
                    height = atof(getAttrValue);
                }
            }
            if (attr->next == NULL)
            {
                //check for null

                Rectangle *rectangle = createRectangleObject(x, y, width, height, parsedValue->unit);
                deleteValue(parsedValue);
                //  printf("\n%s", rectangle -> units);
                xmlAttr *_attr;
                Group *grpList = data;
                SVGimage *list = data;

                for (_attr = cur_node->properties; _attr != NULL; _attr = _attr->next)
                {
                    xmlNode *snapshot = _attr->children;
                    char *getAttrValue = (char *)snapshot->content;
                    char *getAttrName = (char *)_attr->name;

                    if (strcmp("x", getAttrName) != 0 && strcmp("y", getAttrName) != 0 && strcmp("width", getAttrName) != 0 && strcmp("height", getAttrName) != 0)
                    {
                        if (version == 1)
                        {
                          Attribute *attribute1 = createAttribute(getAttrName, getAttrValue);
                            insertBack(rectangle->otherAttributes, attribute1);
                        }
                        else
                        {
                            Attribute *attribute1 = createAttribute(getAttrName, getAttrValue);
                            insertBack(rectangle->otherAttributes, attribute1);
                        }
                    }
                }
                if (version == 1)
                {
                    insertBack(grpList->rectangles, rectangle);
                }
                else
                {

                    insertBack(list->rectangles, rectangle);
                }
            }
        }
    }
}
void insertPath(void *data, xmlNode *cur_node, int version)
{
    int i = 0;
    //printf("%s");
    xmlAttr *attr;
    char *d = NULL;
    for (attr = cur_node->properties; attr != NULL; attr = attr->next)
    {

        i++;
        // printf("%d\n",i);
        xmlNode *snapshot = attr->children;
        char *getAttrValue = (char *)snapshot->content;
        char *getAttrName = (char *)attr->name;
        if (getAttrValue != NULL && getAttrName != NULL)
        {
            if (strcmp("d", getAttrName) == 0)
            {
                d = calloc(1, strlen(getAttrValue) + 1);
                strcpy(d, getAttrValue);
            }

            if (attr->next == NULL)
            {

                Path *path = createPathObject(d);
                xmlAttr *_attr;
                Group *grpList = data;
                SVGimage *list = data;

                for (_attr = cur_node->properties; _attr != NULL; _attr = _attr->next)
                {
                    xmlNode *snapshot = _attr->children;
                    char *getAttrValue = (char *)snapshot->content;
                    char *getAttrName = (char *)_attr->name;

                    if (strcmp("d", getAttrName) != 0)
                    {
                        Attribute *attribute = createAttribute(getAttrName, getAttrValue);
                        insertBack(path->otherAttributes, attribute);
                    }
                }

                //removes data and otherAttributes from memory

                if (version == 1)
                {
                    insertBack(grpList->paths, path);
                }
                else
                {

                    insertBack(list->paths, path);
                }
            }
        }
    }
}

void insertCircle(void *data, xmlNode *cur_node, int version)
{
    int i = 0;
    //printf("%s");
    xmlAttr *attr;
    float cx = 0, cy = 0, r = 0;
    ParsedValue *parsedValue = NULL;

    for (attr = cur_node->properties; attr != NULL; attr = attr->next)
    {

        i++;
        // printf("%d\n",i);
        xmlNode *snapshot = attr->children;
        char *getAttrValue = (char *)snapshot->content;
        char *getAttrName = (char *)attr->name;
        if (getAttrValue != NULL && getAttrName != NULL)
        {

            //seperate them

            if (strcmp("cx", getAttrName) == 0)
            {
                if (parsedValue == NULL)
                {
                    parsedValue = createValue(getAttrValue);
                    cx = parsedValue->value;
                }
                else
                {
                    cx = atof(getAttrValue);
                }
            }

            else if (strcmp("cy", getAttrName) == 0)
            {
                if (parsedValue == NULL)
                {
                    parsedValue = createValue(getAttrValue);
                    cy = parsedValue->value;
                }
                else
                {
                    cy = atof(getAttrValue);
                }
            }
            else if (strcmp("r", getAttrName) == 0)
            {
                if (parsedValue == NULL)
                {
                    parsedValue = createValue(getAttrValue);
                    r = parsedValue->value;
                }
                else
                {
                    r = atof(getAttrValue);
                }
            }

            if (attr->next == NULL)
            {
                //check for null

                Circle *circle = createCircleObject(cx, cy, r, parsedValue->unit);
                deleteValue(parsedValue);

                xmlAttr *_attr;
                Group *grpList = data;
                SVGimage *list = data;
                for (_attr = cur_node->properties; _attr != NULL; _attr = _attr->next)
                {
                    xmlNode *snapshot = _attr->children;
                    char *getAttrValue = (char *)snapshot->content;
                    char *getAttrName = (char *)_attr->name;

                    if (strcmp("cx", getAttrName) != 0 && strcmp("cy", getAttrName) != 0 && strcmp("r", getAttrName) != 0 && strcmp("units", getAttrName) != 0)
                    {
                        Attribute *attribute = createAttribute(getAttrName, getAttrValue);
                        insertBack(circle->otherAttributes, attribute);
                    }
                }

                if (version == 1)
                {
                    insertBack(grpList->circles, circle);
                }
                else
                {

                    insertBack(list->circles, circle);
                }
            }

            // insertBack(tempList -> circle, circle);
        }
    }
}

SVGimage *initializeObjects()
{
    SVGimage *list = NULL;
    list = malloc(sizeof(SVGimage));

    list->paths = initializeList(pathToString, deletePath, comparePaths);
    list->circles = initializeList(circleToString, deleteCircle, compareCircles);
    list->rectangles = initializeList(rectangleToString, deleteRectangle, compareRectangles);
    ;
    list->groups = initializeList(groupToString, deleteGroup, compareGroups);
    list->otherAttributes = initializeList(attributeToString, deleteAttribute, compareAttributes);

    //initialize
    return list;
}

void deleteSVGimage(SVGimage *img)
{
    if (img == NULL || img->paths == NULL || img->rectangles == NULL || img->groups == NULL || img->otherAttributes == NULL)
    {
        return;
    }
    freeList(img->paths);
    freeList(img->circles);
    freeList(img->rectangles);
    freeList(img->groups);
    freeList(img->otherAttributes);
    free(img);
}
//helper functions

Rectangle *createRectangleObject(float x, float y, float width, float height, char units[50])
{
    Rectangle *rect = malloc(sizeof(Rectangle));
    rect->x = x;
    rect->y = y;
    rect->width = width;
    rect->height = height;
    rect->otherAttributes = initializeList(attributeToString, deleteAttribute, compareAttributes);

    char *toBeInserted = calloc(strlen(units) + 4, sizeof(char *));

    if (strlen(units) == 0)
    {
        strcpy(toBeInserted, " ");
        strcpy(rect->units, toBeInserted);
    }
    else
    {
        strcpy(rect->units, units);
    }
    free(toBeInserted);
    return rect;
}

Group *createGroupObject()
{
    Group *group = malloc(sizeof(Group));

    group->otherAttributes = initializeList(attributeToString, deleteAttribute, compareAttributes);

    group->groups = initializeList(attributeToString, deleteGroup, compareAttributes);
    group->rectangles = initializeList(rectangleToString, deleteRectangle, compareRectangles);

    group->circles = initializeList(circleToString, deleteCircle, compareCircles);
    group->paths = initializeList(pathToString, deletePath, comparePaths);

    return group;
}
void deleteGroup(void *data)
{
    if (data == NULL)
    {
        return;
    }
    Group *group = (Group *)data;

    freeList(group->otherAttributes);
    freeList(group->groups);

    freeList(group->rectangles);
    freeList(group->circles);
    freeList(group->paths);
    free(group);
}
//Parses objects with values aside the common integer e.g 14px
ParsedValue *createValue(char *data)
{
    ParsedValue *value = malloc(sizeof(ParsedValue));
    char *temp;
    char *end;

    temp = calloc(1, strlen(data) + 1);
    strcpy(temp, data);
    value->value = strtof(temp, &end);
    value->unit = calloc(1, strlen(data) + 1);
    strcpy(value->unit, end);

    free(temp);
    return value;
}

void deleteValue(ParsedValue *data)
{
    free(data->unit);
    free(data);
}

Attribute *createAttribute(char *name, char *value)
{
    Attribute *this = malloc(sizeof(Attribute));
    this->name = calloc(strlen(name) + 4, sizeof(char *));
    this->value = calloc(strlen(value) + 4, sizeof(char *));

    //TO DO
    if (name == NULL || value == NULL)
    {
        char *str = calloc(4, sizeof(char));
        strcpy(str, "EMPTYY");
        strcpy(this->name, str);
        strcpy(this->value, str);
        free(str);
        return this;
    }

    strcpy(this->name, name);
    strcpy(this->value, value);

    return this;
}

///Create circle doesnt break code when otherAttributes is null
Circle *createCircleObject(float cx, float cy, float r, char units[50])
{
    Circle *circle = malloc(sizeof(Circle));

    circle->cx = cx;
    circle->cy = cy;
    circle->r = r;
    //added

    circle->otherAttributes = initializeList(attributeToString, deleteAttribute, compareAttributes);
    char *toBeInserted = calloc(strlen(units) + 4, sizeof(char *));

    if (strlen(units) == 0 || units == NULL)
    {
        strcpy(toBeInserted, " ");
        strcpy(circle->units, toBeInserted);
    }
    else
    {

        strcpy(toBeInserted, units);
        strcpy(circle->units, toBeInserted);
    }

    //might cause a double free?
    free(toBeInserted);
    //   deleteAttribute(dummyValue);
    return circle;
}
Path *createPathObject(char *data)
{

    if (data == NULL)
    {
        strcpy(data, " ");
    }
    Path *path = malloc(sizeof(Path));
    path->data = data;
    path->otherAttributes = initializeList(attributeToString, deleteAttribute, compareAttributes);

    return path;
}
//helper functions

void deleteAttribute(void *data)
{

    if (data == NULL)
    {
        return;
    }
    Attribute *this = data;
    free(this->name);
    free(this->value);
    free(this);
}
char *attributeToString(void *data)
{

    char *temp = malloc(10);
    strcpy(temp, " ");
    if (data == NULL)
    {
        return temp;
    }
    return temp;
    /*
    Attribute *newData = (Attribute *)data;
    char *newBuffer = (char *)calloc(2,sizeof(Attribute) + 10);
    strcat(newBuffer, newData->name);
    strcat(newBuffer, "=");
    strcat(newBuffer, newData->value);
    strcat(newBuffer, "\0");
    */
}

int compareAttributes(const void *first, const void *second)
{
    return 0;
}
void deleteCircle(void *data)
{
    if (data == NULL)
    {
        return;
    }
    Circle *this = data;
    freeList(this->otherAttributes);
    free(this);
}

char *circleToString(void *data)
{
    char *temp = malloc(10);
    strcpy(temp, " ");
    if (data == NULL)
    {
        return temp;
    }

    /*
      Circle *this = data;
        char * value = NULL;
        value = calloc(1,  500);
    char *dummy = calloc(1, 500);
    sprintf(dummy, "%.1f",  this -> cx);
    strcat(value, "\ncx:\t");
    strcat(value, dummy);
   
    sprintf(dummy, "%.1f",  this -> cy);
    strcat(value, "\t\tcy:\t");
    strcat(value, dummy);

        sprintf(dummy, "%.1f",  this -> r);
        strcat(value, "\t\tr:\t");
        strcat(value, dummy);
    strcat(value, "\t\tunit:\t");

    strcpy(dummy, this -> units);
    strcat(value, "\notherAttributes:");

        strcat(value, dummy);
    while (this -> otherAttributes -> head != NULL) {
        Attribute *attribute = (Attribute *) this -> otherAttributes -> head -> data;
        strcat(value, attribute -> name);
        strcat(value, ":\t");
        strcat(value, attribute -> value);
        strcat(value, "\t");


        this -> otherAttributes -> head = this -> otherAttributes -> head -> next;
    }
    strcat(value, "\n");

    
    free(dummy);
    */

    return temp;
}

int compareCircles(const void *first, const void *second)
{

    return 0;
}

void deletePath(void *data)
{
    if (data == NULL)
    {
        return;
    }
    Path *this = data;
    free(this->data);
    freeList(this->otherAttributes);
    free(this);
}
char *pathToString(void *data)
{
    char *temp = malloc(10);
    strcpy(temp, " ");
    if (data == NULL)
    {
        return temp;
    }
    return temp;
}
int comparePaths(const void *first, const void *second) { return 0; }

void deleteRectangle(void *data)
{
    if (data == NULL)
    {
        return;
    }
    Rectangle *this = data;
    freeList(this->otherAttributes);
    free(this);
}
char *rectangleToString(void *data)
{

    char *temp = malloc(10);
    strcpy(temp, " ");
    if (data == NULL)
    {
        return temp;
    }
    return temp;
    /*
    Rectangle *this = data;
          char * value = NULL;
          value = calloc(1,  500);
      char *dummy = calloc(1, 500);
   
    sprintf(dummy, "%.1f",  this -> x);
      strcat(value, "\nx:\t");
      strcat(value, dummy);
     
      sprintf(dummy, "%.1f",  this -> y);
      strcat(value, "\t\ty:\t");
      strcat(value, dummy);
    
  

          sprintf(dummy, "%.1f",  this -> width );
          strcat(value, "\t\twidth:\t");
          strcat(value, dummy);
    
    sprintf(dummy, "%.1f",  this -> height );
    strcat(value, "\t\theight:\t");
    strcat(value, dummy);
    
    
      strcat(value, "\notherAttributes:");

          strcat(value, dummy);
      while (this -> otherAttributes -> head != NULL) {
          Attribute *attribute = (Attribute *) this -> otherAttributes -> head -> data;
          strcat(value, attribute -> name);
          strcat(value, ":\t");
          strcat(value, attribute -> value);
          strcat(value, "\t");


          this -> otherAttributes -> head = this -> otherAttributes -> head -> next;
      }
      strcat(value, "\n");

      free(dummy);
      
    return value;
    */
}
int compareRectangles(const void *first, const void *second) { return 0; }

char *groupToString(void *data)
{

    char *temp = malloc(10);
    strcpy(temp, " ");
    if (data == NULL)
    {
        return temp;
    }
    return temp;
}
int compareGroups(const void *first, const void *second)
{
    return 0;
};

void validateNameSpace(char *namespace, SVGimage **list)
{
    SVGimage *temp_list = *list;
    if (namespace != NULL)
    {

        if (strlen((char *)namespace) < 256)
        {
            strcpy(temp_list->namespace, (char *)namespace);
        }
        else
        {

            long length = strlen((char *)namespace) - 256;
            strncpy(temp_list->namespace, (char *)namespace, 256 - length);
        }
    }
    else
    {
        strcpy(temp_list->namespace, " ");
    }
}

void validateTitle(char *title, SVGimage **list)
{
    SVGimage *temp_list = *list;
    if (title != NULL)
    {

        if (strlen((char *)title) < 256)
        {
            strcpy(temp_list->title, (char *)title);
        }
        else
        {

            long length = strlen((char *)title) - 256;
            strncpy(temp_list->title, (char *)title, 256 - length);
        }
    }
    else
    {
        strcpy(temp_list->title, " ");
    }
}

void validateDescription(char *description, SVGimage **list)
{
    SVGimage *temp_list = *list;
    if (description != NULL)
    {

        if (strlen((char *)description) < 256)
        {
            strcpy(temp_list->description, (char *)description);
        }
        else
        {

            long length = strlen((char *)description) - 256;
            strncpy(temp_list->description, (char *)description, 256 - length);
        }
    }
    else
    {
        strcpy(temp_list->description, " ");
    }
}

int StartsWith(const char *a, const char *b)
{
    if (strncmp(a, b, strlen(b)) == 0)
        return 1;
    return 0;
}
