#pragma once
#include <string>

using namespace std;

struct Tag {

    Tag(string tag = "") : tag(tag) {

    }

    string tag;

    void setTag(string newTag) {
        tag = newTag;
    }

    string getTag() {
		return tag;
	}

};