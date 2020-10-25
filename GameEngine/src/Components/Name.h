#pragma once

#include <string>

using namespace std;

struct Name {

    Name(string entityName) : entityName(entityName){

    }

    string entityName;

    void setName(string s) {
		entityName = s;
	}

    string getName() {
		return entityName;
	}

    bool checkNameMatch(string s){
        int result = entityName.compare(s);
        if(result == 0){
            return true; // name matches
        } else {
            return false; // name does not match
        }
    }

};