#include <iostream>
#include "LinearProbeHashTable.h"

void testHash();

bool checkTest(std::string, int, int);
bool checkTest(std::string, std::string, std::string);
int main()
{
    std::cout << "Linear Hashing!" << std::endl;
    testHash();
    return 0;
}


void testHash(){
    LinearHashtable<int> htable(9);

    htable.add(9);
    checkTest("#1 Adding value: ", 9, htable[0]);
    htable.add(200);
    checkTest("#2 Adding value: ", 200, htable[2]);
    htable.add(18);
    checkTest("#3 Checking Collision: ", 18, htable[1]);
    htable.add(27);
    checkTest("#4 Checking Multiple Collisions: ", 27, htable[3]);

    checkTest("#5 Checking Contains()", true, htable.contains(9));

    htable.remove(9);
    checkTest("#6 Checking Remove: ", false, htable.contains(9));
    htable.add(8);
    htable.add(17);
    checkTest("#7 Checking Replacing: ", 17, htable[0]);

    for(int i = 0; i < 4; i++){
        htable.add(i* 5);
    }

    bool test = false;
    try{
        htable.add(5);
    }
    catch(std::exception e){
        test = true;
    }
     checkTest("#8 Adding to a full Hashtable: ", true, test);
    std::cout << htable;

}


//This helps with testing, do not modify.
bool checkTest(std::string testName, std::string whatItShouldBe, std::string whatItIs) {

  if (whatItShouldBe == whatItIs) {
    std::cout << "Passed " << testName << std::endl;
    return true;
  }
  else {
    std::cout << "****** Failed test " << testName << " ****** " << std::endl << "     Output was " << whatItIs << std::endl << "     Output should have been " << whatItShouldBe << std::endl;
    return false;
  }
}

//This helps with testing, do not modify.
bool checkTest(std::string testName, int whatItShouldBe, int whatItIs) {

  if (whatItShouldBe == whatItIs) {
    std::cout << "Passed " << testName << std::endl;
    return true;
  }
  else {
    std::cout << "****** Failed test " << testName << " ****** " << std::endl << "     Output was " << whatItIs << std::endl << "     Output should have been " << whatItShouldBe << std::endl;
    return false;
  }
}
