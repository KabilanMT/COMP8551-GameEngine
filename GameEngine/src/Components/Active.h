#pragma once

struct Active {

    Active(bool isActive = true) : isActive(isActive){

    }

    bool isActive = true;

    void makeActive() {
		isActive = true;
	}

    void makeInactive() {
		isActive = false;
	}

    void toggleActiveStatus(){
        isActive = !isActive;
    }

    void setActiveStatus(bool a) {
		isActive = a;
	}

    bool getIfActive() {
		return isActive;
	}

};