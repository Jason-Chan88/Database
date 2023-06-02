#include "vdetect.h"
#include <random>
#include <vector>
enum RANDOM {UNIFORMINT, UNIFORMREAL, NORMAL};
class Random {
public:
    Random(int min, int max, RANDOM type=UNIFORMINT, int mean=50, int stdev=20) : m_min(min), m_max(max), m_type(type)
    {
        if (type == NORMAL){
            //the case of NORMAL to generate integer numbers with normal distribution
            m_generator = std::mt19937(m_device());
            //the data set will have the mean of 50 (default) and standard deviation of 20 (default)
            //the mean and standard deviation can change by passing new values to constructor
            m_normdist = std::normal_distribution<>(mean,stdev);
        }
        else if (type == UNIFORMINT) {
            //the case of UNIFORMINT to generate integer numbers
            // Using a fixed seed value generates always the same sequence
            // of pseudorandom numbers, e.g. reproducing scientific experiments
            // here it helps us with testing since the same sequence repeats
            m_generator = std::mt19937(10);// 10 is the fixed seed value
            m_unidist = std::uniform_int_distribution<>(min,max);
        }
        else{ //the case of UNIFORMREAL to generate real numbers
            m_generator = std::mt19937(10);// 10 is the fixed seed value
            m_uniReal = std::uniform_real_distribution<double>((double)min,(double)max);
        }
    }
    void setSeed(int seedNum){
        // we have set a default value for seed in constructor
        // we can change the seed by calling this function after constructor call
        // this gives us more randomness
        m_generator = std::mt19937(seedNum);
    }

    int getRandNum(){
        // this function returns integer numbers
        // the object must have been initialized to generate integers
        int result = 0;
        if(m_type == NORMAL){
            //returns a random number in a set with normal distribution
            //we limit random numbers by the min and max values
            result = m_min - 1;
            while(result < m_min || result > m_max)
                result = m_normdist(m_generator);
        }
        else if (m_type == UNIFORMINT){
            //this will generate a random number between min and max values
            result = m_unidist(m_generator);
        }
        return result;
    }

    double getRealRandNum(){
        // this function returns real numbers
        // the object must have been initialized to generate real numbers
        double result = m_uniReal(m_generator);
        // a trick to return numbers only with two deciaml points
        // for example if result is 15.0378, function returns 15.03
        // to round up we can use ceil function instead of floor
        result = std::floor(result*100.0)/100.0;
        return result;
    }

private:
    int m_min;
    int m_max;
    RANDOM m_type;
    std::random_device m_device;
    std::mt19937 m_generator;
    std::normal_distribution<> m_normdist;//normal distribution
    std::uniform_int_distribution<> m_unidist;//integer uniform distribution
    std::uniform_real_distribution<double> m_uniReal;//real uniform distribution

};
class Tester{
public:

    bool testInsertCases();
    bool testGetVirusErrorCase();
    bool testGetVirusNonCollidingKeys();
    bool testGetVirusCollidingKeys();
    bool testRemoveNonCollidingKeys();
    bool testRemoveCollidingKeys();
    bool testRehashInsertion();
    bool testRehashLoadFactor();
    bool testRehashRemoval();
    bool testRehashDeleteRatio();

};

unsigned int hashCode(const string str);
string sequencer(int size, int seedNum);

int main(){
    Tester tester;

    if (tester.testInsertCases()) // should return true
        cout << "\ttestInsertCases() returned true." << endl;
    else
        cout << "\ttestInsertCases() returned false." << endl;

    if (tester.testGetVirusErrorCase()) // should return true
        cout << "\ttestGetVirusErrorCase() returned true." << endl;
    else
        cout << "\ttestGetVirusErrorCase() returned false." << endl;

    if (tester.testGetVirusNonCollidingKeys()) // should return true
        cout << "\ttestGetVirusNonCollidingKeys() returned true." << endl;
    else
        cout << "\ttestGetVirusNonCollidingKeys() returned false." << endl;

    if (tester.testGetVirusCollidingKeys()) // should return true
        cout << "\ttestGetVirusCollidingKeys() returned true." << endl;
    else
        cout << "\ttestGetVirusCollidingKeys() returned false." << endl;

    if (tester.testRemoveNonCollidingKeys()) // should return true
        cout << "\ttestRemoveNonCollidingKeys() returned true." << endl;
    else
        cout << "\ttestRemoveNonCollidingKeys() returned false." << endl;

    if (tester.testRemoveCollidingKeys()) // should return true
        cout << "\ttestRemoveCollidingKeys() returned true." << endl;
    else
        cout << "\ttestRemoveCollidingKeys() returned false." << endl;

    if (tester.testRehashInsertion()) // should return true
        cout << "\ttestRehashInsertion() returned true." << endl;
    else
        cout << "\ttestRehashInsertion() returned false." << endl;

    if (tester.testRehashLoadFactor()) // should return true
        cout << "\ttestRehashLoadFactor() returned true." << endl;
    else
        cout << "\ttestRehashLoadFactor() returned false." << endl;

    if (tester.testRehashRemoval()) // should return true
        cout << "\ttestRehashRemoval() returned true." << endl;
    else
        cout << "\ttestRehashRemoval() returned false." << endl;

    if (tester.testRehashDeleteRatio()) // should return true
        cout << "\ttestRehashDeleteRatio() returned true." << endl;
    else
        cout << "\ttestRehashDeleteRatio() returned false." << endl;

    vector<Virus> dataList;
    Random RndID(MINID,MAXID);
    VDetect vdetect(MINPRIME, hashCode, DOUBLEHASH);
    bool result = true;
    for (int i=0;i<49;i++){
        // generating random data
        Virus dataObj = Virus(sequencer(5, i), RndID.getRandNum());
        // saving data for later use
        dataList.push_back(dataObj);
        // inserting data in to the VDetect object
        vdetect.insert(dataObj);
    }
    // dumping the data points to the standard output
    vdetect.dump();

    // checking whether all data are inserted
    for (vector<Virus>::iterator it = dataList.begin(); it != dataList.end(); it++){
        result = result && (*it == vdetect.getVirus((*it).getKey(), (*it).getID()));
    }

    if (result)
        cout << "All data points exist in the VDetect object!\n";
    else
        cout << "Some data points are missing in the VDetect object\n";

    return 0;
}

unsigned int hashCode(const string str) {
    unsigned int val = 0 ;
    const unsigned int thirtyThree = 33 ;  // magic number from textbook
    for ( int i = 0 ; i < str.length(); i++)
        val = val * thirtyThree + str[i] ;
    return val ;
}

string sequencer(int size, int seedNum){
    //this function returns a random DNA sequence
    string sequence = "";
    Random rndObject(0,3);
    rndObject.setSeed(seedNum);
    for (int i=0;i<size;i++){
        sequence = sequence + ALPHA[rndObject.getRandNum()];
    }
    return sequence;
}

//Function: Tester::testInsertCases
//Case: Insert 5 nodes with different keys so none collide, test the size is correct, test that the data
// object is in the Vdetect, than test the index is correct and that the object is there
//Expected result: we expect this to return true as it should past the test case
bool Tester::testInsertCases() {
    vector<Virus> dataList;
    Random RndID(MINID,MAXID);
    VDetect vdetect(MINPRIME, hashCode, NONE);
    bool result = true;

    Virus dataObj = Virus("A" , 1000); // insert with different keys
    dataList.push_back(dataObj);
    vdetect.insert(dataObj);

    Virus dataObj1 = Virus("B" , 1001);
    dataList.push_back(dataObj1);
    vdetect.insert(dataObj1);

    Virus dataObj2 = Virus("C" , 1002);
    dataList.push_back(dataObj2);
    vdetect.insert(dataObj2);

    Virus dataObj3 = Virus("D" , 1003);
    dataList.push_back(dataObj3);
    vdetect.insert(dataObj3);

    Virus dataObj4 = Virus("E" , 1004);
    dataList.push_back(dataObj4);
    vdetect.insert(dataObj4);

    result = result && (vdetect.m_currentSize == 5); // check size

    result = result && ((vdetect.getVirus("A", 1000) == dataObj)); // checks it's inside the Vdetect

    result = result && ((vdetect.m_currentTable[vdetect.m_hash("A")] == dataObj)); // checks if object at proper index

    // checking whether all data are inserted
    for (vector<Virus>::iterator it = dataList.begin(); it != dataList.end(); it++){
        result = result && (*it == vdetect.getVirus((*it).getKey(), (*it).getID()));
    }

    return result;
}

//Function: Tester::testGetVirusErrorCase
//Case: Insert 5 nodes with different keys so none collide, try to find an object that doesn't exist
//Expected result: we expect this to return true as it shouldn't find the virus
bool Tester::testGetVirusErrorCase() {
    vector<Virus> dataList;
    Random RndID(MINID,MAXID);
    VDetect vdetect(MINPRIME, hashCode, NONE);
    bool result = true;

    Virus dataObj = Virus("A" , 1000);
    dataList.push_back(dataObj);
    vdetect.insert(dataObj);

    Virus dataObj1 = Virus("B" , 1001);
    dataList.push_back(dataObj1);
    vdetect.insert(dataObj1);

    Virus dataObj2 = Virus("C" , 1002);
    dataList.push_back(dataObj2);
    vdetect.insert(dataObj2);

    Virus dataObj3 = Virus("D" , 1003);
    dataList.push_back(dataObj3);
    vdetect.insert(dataObj3);

    Virus dataObj4 = Virus("E" , 1004);
    dataList.push_back(dataObj4);
    vdetect.insert(dataObj4);

    result = result && (vdetect.m_currentSize == 5);

    result = result && ((vdetect.getVirus("F", 1005) == EMPTY)); // it should equal empty as the object don't exist

    result = result && ((vdetect.m_currentTable[vdetect.m_hash("A")] == dataObj));

    // checking whether all data are inserted
    for (vector<Virus>::iterator it = dataList.begin(); it != dataList.end(); it++){
        result = result && (*it == vdetect.getVirus((*it).getKey(), (*it).getID()));
    }

    return result;
}

//Function: Tester::testGetVirusNonCollidingKeys
//Case: Insert 5 nodes with different keys so none collide, test if we can find the object
//Expected result: we expect this to return true as it should past the test case
bool Tester::testGetVirusNonCollidingKeys() {
    vector<Virus> dataList;
    Random RndID(MINID,MAXID);
    VDetect vdetect(MINPRIME, hashCode, NONE);
    bool result = true;

    Virus dataObj = Virus("A" , 1000);
    dataList.push_back(dataObj);
    vdetect.insert(dataObj);

    Virus dataObj1 = Virus("B" , 1001);
    dataList.push_back(dataObj1);
    vdetect.insert(dataObj1);

    Virus dataObj2 = Virus("C" , 1002);
    dataList.push_back(dataObj2);
    vdetect.insert(dataObj2);

    Virus dataObj3 = Virus("D" , 1003);
    dataList.push_back(dataObj3);
    vdetect.insert(dataObj3);

    Virus dataObj4 = Virus("E" , 1004);
    dataList.push_back(dataObj4);
    vdetect.insert(dataObj4);

    result = result && ((vdetect.getVirus("C", 1002) == dataObj2)); // should be able to find this

    // checking whether all data are inserted
    for (vector<Virus>::iterator it = dataList.begin(); it != dataList.end(); it++){
        result = result && (*it == vdetect.getVirus((*it).getKey(), (*it).getID()));
    }

    return result;
}

//Function: Tester::testGetVirusCollidingKeys
//Case: Insert 5 nodes with the same key so we have to deal with colliding, test if we can find that object
//Expected result: we expect this to return true as it should past the test case
bool Tester::testGetVirusCollidingKeys() {
    vector<Virus> dataList;
    Random RndID(MINID,MAXID);
    VDetect vdetect(MINPRIME, hashCode,QUADRATIC);
    bool result = true;

    Virus dataObj = Virus("A" , 1000);
    dataList.push_back(dataObj);
    vdetect.insert(dataObj);

    Virus dataObj1 = Virus("A" , 1001);
    dataList.push_back(dataObj1);
    vdetect.insert(dataObj1);

    Virus dataObj2 = Virus("A" , 1002);
    dataList.push_back(dataObj2);
    vdetect.insert(dataObj2);

    Virus dataObj3 = Virus("A" , 1003);
    dataList.push_back(dataObj3);
    vdetect.insert(dataObj3);

    Virus dataObj4 = Virus("A" , 1004);
    dataList.push_back(dataObj4);
    vdetect.insert(dataObj4);

    result = result && ((vdetect.getVirus("A", 1003) == dataObj3)); // should be able to find this

    for(int i = 0; i < dataList.size(); i++){

    }

    // checking whether all data are inserted
//    for (vector<Virus>::iterator it = dataList.begin(); it != dataList.end(); it++){
//        result = result && (*it == vdetect.getVirus((*it).getKey(), (*it).getID()));
//    }

    return result;
}

//Function: Tester::testRemoveNonCollidingKeys
//Case: Insert 5 nodes with different keys so none collide, test removal of a object
//Expected result: we expect this to return true as it should past the test case
bool Tester::testRemoveNonCollidingKeys() {
    vector<Virus> dataList;
    Random RndID(MINID,MAXID);
    VDetect vdetect(MINPRIME, hashCode, NONE);
    bool result = true;

    Virus dataObj = Virus("A" , 1000);
    dataList.push_back(dataObj);
    vdetect.insert(dataObj);

    Virus dataObj1 = Virus("B" , 1001);
    dataList.push_back(dataObj1);
    vdetect.insert(dataObj1);

    Virus dataObj2 = Virus("C" , 1002);
    dataList.push_back(dataObj2);
    vdetect.insert(dataObj2);

    Virus dataObj3 = Virus("D" , 1003);
    dataList.push_back(dataObj3);
    vdetect.insert(dataObj3);

    Virus dataObj4 = Virus("E" , 1004);
    dataList.push_back(dataObj4);
    vdetect.insert(dataObj4);

    // checking whether all data are inserted
    for (vector<Virus>::iterator it = dataList.begin(); it != dataList.end(); it++){
        result = result && (*it == vdetect.getVirus((*it).getKey(), (*it).getID()));
    }

    result = result && (vdetect.m_currentSize == 5);

    result = result && ((vdetect.m_currentTable[vdetect.m_hash("E")] == dataObj4));

    vdetect.remove(dataObj4);

    result = result && (vdetect.m_currNumDeleted == 1); // test the num deleted went up 1 for removal

    result = result && ((vdetect.m_currentTable[vdetect.m_hash("E")] == DELETED)); // test that spot is now declared as deleted

    return result;
}

//Function: Tester::testRemoveCollidingKeys
//Case: Insert 5 nodes with the same keys so they collide and test remove of a object
//Expected result: we expect this to return true as it should past the test case
bool Tester::testRemoveCollidingKeys() {
    vector<Virus> dataList;
    Random RndID(MINID,MAXID);
    VDetect vdetect(MINPRIME, hashCode, QUADRATIC);
    bool result = true;

    Virus dataObj = Virus("A" , 1000);
    dataList.push_back(dataObj);
    vdetect.insert(dataObj);

    Virus dataObj1 = Virus("A" , 1001);
    dataList.push_back(dataObj1);
    vdetect.insert(dataObj1);

    Virus dataObj2 = Virus("A" , 1002);
    dataList.push_back(dataObj2);
    vdetect.insert(dataObj2);

    Virus dataObj3 = Virus("A" , 1003);
    dataList.push_back(dataObj3);
    vdetect.insert(dataObj3);

    Virus dataObj4 = Virus("A" , 1004);
    dataList.push_back(dataObj4);
    vdetect.insert(dataObj4);

    // checking whether all data are inserted
    for (vector<Virus>::iterator it = dataList.begin(); it != dataList.end(); it++){
        result = result && (*it == vdetect.getVirus((*it).getKey(), (*it).getID()));
    }

    result = result && (vdetect.m_currentSize == 5);

    result = result && ((vdetect.getVirus(dataObj4.m_key, dataObj4.m_id) == dataObj4));

    vdetect.remove(dataObj4);

    result = result && (vdetect.m_currNumDeleted == 1); // checks deleted went up

    result = result && ((vdetect.m_currentTable[81] == DELETED)); //checks if that spot hold the virus which it should

    return result;
}

//Function: Tester::testRehashInsertion
//Case: Insert 51 nodes so it rehashes but not to completion and test that it rehashed
//Expected result: we expect this to return true as it should past the test case
bool Tester::testRehashInsertion() {
    vector<Virus> dataList;
    Random RndID(MINID,MAXID);
    VDetect vdetect(MINPRIME, hashCode, DOUBLEHASH);
    bool result = true;

    for (int i=0;i<51;i++){
        // generating random data
        Virus dataObj = Virus(sequencer(5, i), RndID.getRandNum());
        // saving data for later use
        dataList.push_back(dataObj);
        // inserting data in to the VDetect object
        vdetect.insert(dataObj);

    }

    result = result && (vdetect.m_oldTable != nullptr && vdetect.m_currentTable != nullptr); // checks if both exist they should

    result = result && (vdetect.m_currentCap == vdetect.findNextPrime((vdetect.m_oldSize - 0) * 4)); // checks rehashing has started

    // checking whether all data are inserted
    for (vector<Virus>::iterator it = dataList.begin(); it != dataList.end(); it++){
        result = result && (*it == vdetect.getVirus((*it).getKey(), (*it).getID()));
    }

    return result;
}

//Function: Tester::testRehashLoadFactor
//Case: Insert 58 and test that it rehashed completely
//Expected result: we expect this to return true as it should past the test case
bool Tester::testRehashLoadFactor() {
    vector<Virus> dataList;
    Random RndID(MINID,MAXID);
    VDetect vdetect(MINPRIME, hashCode, DOUBLEHASH);
    bool result = true;

    for (int i=0;i<58;i++){
        // generating random data
        Virus dataObj = Virus(sequencer(5, i), RndID.getRandNum());
        // saving data for later use
        dataList.push_back(dataObj);
        // inserting data in to the VDetect object
        vdetect.insert(dataObj);

    }

    result = result && (vdetect.m_currentCap == vdetect.findNextPrime((vdetect.m_oldSize - 0) * 4)); // checks rehashing has started


    result = result && (vdetect.m_oldTable == nullptr); // checks if old table doesn't exist


    // checking whether all data are inserted
    for (vector<Virus>::iterator it = dataList.begin(); it != dataList.end(); it++){
        result = result && (*it == vdetect.getVirus((*it).getKey(), (*it).getID()));
    }

    return result;
}

//Function: Tester::testRehashRemoval
//Case: Insert 10 nodes with different keys and removes 9 of them than test if rehash is in progress
//Expected result: we expect this to return true as it should past the test case
bool Tester::testRehashRemoval() {
    vector<Virus> dataList;
    Random RndID(MINID,MAXID);
    VDetect vdetect(MINPRIME, hashCode, DOUBLEHASH);
    bool result = true;

    Virus dataObj = Virus("A" , 1000);
    dataList.push_back(dataObj);
    vdetect.insert(dataObj);

    Virus dataObj1 = Virus("B" , 1001);
    dataList.push_back(dataObj1);
    vdetect.insert(dataObj1);

    Virus dataObj2 = Virus("C" , 1002);
    dataList.push_back(dataObj2);
    vdetect.insert(dataObj2);

    Virus dataObj3 = Virus("D" , 1003);
    dataList.push_back(dataObj3);
    vdetect.insert(dataObj3);

    Virus dataObj4 = Virus("E" , 1004);
    dataList.push_back(dataObj4);
    vdetect.insert(dataObj4);

    Virus dataObj5 = Virus("F" , 1005);
    dataList.push_back(dataObj5);
    vdetect.insert(dataObj5);

    Virus dataObj6 = Virus("G" , 1006);
    dataList.push_back(dataObj6);
    vdetect.insert(dataObj6);

    Virus dataObj7 = Virus("H" , 1007);
    dataList.push_back(dataObj7);
    vdetect.insert(dataObj7);

    Virus dataObj8 = Virus("I" , 1008);
    dataList.push_back(dataObj8);
    vdetect.insert(dataObj8);

    Virus dataObj9 = Virus("J" , 1009);
    dataList.push_back(dataObj9);
    vdetect.insert(dataObj9);

    // checking whether all data are inserted
    for (vector<Virus>::iterator it = dataList.begin(); it != dataList.end(); it++){
        result = result && (*it == vdetect.getVirus((*it).getKey(), (*it).getID()));
    }

    vdetect.remove(dataObj1);
    vdetect.remove(dataObj2);
    vdetect.remove(dataObj3);
    vdetect.remove(dataObj4);
    vdetect.remove(dataObj5);
    vdetect.remove(dataObj6);
    vdetect.remove(dataObj7);
    vdetect.remove(dataObj8);
    vdetect.remove(dataObj9);

    //vdetect.dump();

    result = result && (vdetect.m_currentCap == vdetect.findNextPrime((vdetect.m_oldSize - 9) * 4));

    return result;
}

//Function: Tester::testRehashDeleteRatio
//Case: Insert 10 nodes and remove 9 and test that it went to completion
//Expected result: we expect this to return true as it should past the test case
bool Tester::testRehashDeleteRatio() {
    vector<Virus> dataList;
    Random RndID(MINID,MAXID);
    VDetect vdetect(MINPRIME, hashCode, DOUBLEHASH);
    bool result = true;

    Virus dataObj = Virus("A" , 1000);
    dataList.push_back(dataObj);
    vdetect.insert(dataObj);

    Virus dataObj1 = Virus("B" , 1001);
    dataList.push_back(dataObj1);
    vdetect.insert(dataObj1);

    Virus dataObj2 = Virus("C" , 1002);
    dataList.push_back(dataObj2);
    vdetect.insert(dataObj2);

    Virus dataObj3 = Virus("D" , 1003);
    dataList.push_back(dataObj3);
    vdetect.insert(dataObj3);

    Virus dataObj4 = Virus("E" , 1004);
    dataList.push_back(dataObj4);
    vdetect.insert(dataObj4);

    Virus dataObj5 = Virus("F" , 1005);
    dataList.push_back(dataObj5);
    vdetect.insert(dataObj5);

    Virus dataObj6 = Virus("G" , 1006);
    dataList.push_back(dataObj6);
    vdetect.insert(dataObj6);

    Virus dataObj7 = Virus("H" , 1007);
    dataList.push_back(dataObj7);
    vdetect.insert(dataObj7);

    Virus dataObj8 = Virus("I" , 1008);
    dataList.push_back(dataObj8);
    vdetect.insert(dataObj8);

    Virus dataObj9 = Virus("J" , 1009);
    dataList.push_back(dataObj9);
    vdetect.insert(dataObj9);

    // checking whether all data are inserted
    for (vector<Virus>::iterator it = dataList.begin(); it != dataList.end(); it++){
        result = result && (*it == vdetect.getVirus((*it).getKey(), (*it).getID()));
    }

    vdetect.remove(dataObj1);
    vdetect.remove(dataObj2);
    vdetect.remove(dataObj3);
    vdetect.remove(dataObj4);
    vdetect.remove(dataObj5);
    vdetect.remove(dataObj6);
    vdetect.remove(dataObj7);
    vdetect.remove(dataObj8);
    vdetect.remove(dataObj9);


    result = result && (vdetect.m_currentCap == vdetect.findNextPrime((vdetect.m_oldSize - 9) * 4)); // this is how you know it starts

    result = result && (vdetect.m_oldTable == nullptr); // this is how you know it ended

    return result;
}


