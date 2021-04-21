#include <stdio.h>

#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <list> 
#include <iterator>
#include <unordered_map> 
using namespace std;

//./traceanal < ls.slog | sort -nrk 2

int printMiniMap(unordered_map<string, int> *map, string current){
    unordered_map<string, int>:: iterator itr; 
    for (itr = map->begin(); itr != map->end(); itr++) { 
        cout << "  " << current << ": " << itr->first << "  " << itr->second << "\n";
    }  
}

void printSmallMap(unordered_map<string, int> *map){
    int instances = 0;
    int total = 0;
    unordered_map<string, int>:: iterator itr; 
    for (itr = map->begin(); itr != map->end(); itr++) { 
        cout << itr->first << "  " << itr->second << "\n";
        instances++;
        total += itr->second;
    }
    cout << "AAA: "<< total << " invoked system call instances from "<< instances <<" unique system calls\n";  
}

int getMiniMapTotal(unordered_map<string, int> *map){
    int total = 0;
    unordered_map<string, int>:: iterator itr;
    for(itr = map->begin(); itr != map->end(); itr++){
        total += itr->second;
    }
    return total;
}

void printMap(unordered_map<string, unordered_map<string, int>> *map){
    int instances = 0;
    int total = 0;
    int calls = 0;
    unordered_map<string, unordered_map<string, int>>::iterator itr;
    for(itr = map->begin(); itr != map->end(); itr++){
        calls = getMiniMapTotal(&itr->second);
        if(!calls)calls = 1;
        cout << itr->first << " " << calls << "\n";
        total += calls;
        printMiniMap(&itr->second, itr->first);;
        instances++;
    }
    cout << "AAA: "<< total << " invoked system call instances from "<< instances <<" unique system calls\n";
}


void addToMiniMap(string var, unordered_map<string, int> *map){
    if(map->find(var) == map->end()){
        if(var == ""){
            return;
        }
        map->insert(make_pair(var, 1));
    }else{
        int z = map->find(var)->second;
        z++;
        map->at(var) = z;
    }
}

string addToMap(string current, string prev, unordered_map<string, unordered_map<string, int>> *map){
    if(map->find(prev) == map->end()){
        if(prev == ""){
            return current;
        }
        unordered_map<string, int> miniMap;
        addToMiniMap(current, &miniMap);
        map->insert(make_pair(prev, miniMap));
    }else{
        addToMiniMap(current, &map->at(prev));
    }
    return current;
}

string returnSystemCall(char *buffer){
    string smallBuff = "";
    int x = 0;
    for(int i = 0; i < 300; i++){
        char c = buffer[i];
        if(c == ' '){
            smallBuff = "";
            return smallBuff.c_str();
        }else if(c == '('){
            return smallBuff.c_str();
        }else{
            smallBuff.append(string(1,buffer[i]));
            x++;
        }
    }
}

main(int argc, char **argv){
    char buffer[300]; // = (char*)malloc(sizeof(char)*300);
    string current, prev;
    if (argc > 1 && !strcmp(argv[1],"seq")){
        unordered_map<string, unordered_map<string, int>> map;
        while(fgets(buffer, 300, stdin) != NULL){
            current = returnSystemCall(buffer);
            if(prev != ""){
                prev = addToMap(current,prev,&map);
            }else{
                prev = current;
            }
        }
        printMap(&map);
    }else{
        unordered_map<string, int> map;
        while(fgets(buffer, 300, stdin) != NULL){
            current = returnSystemCall(buffer);
            if(current != ""){
                addToMiniMap(current,&map);
            }
        }
        printSmallMap(&map);
    }
    
}