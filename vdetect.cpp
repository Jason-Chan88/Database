#include "vdetect.h"
VDetect::VDetect(int size, hash_fn hash, prob_t probing = DEFPOLCY){
    if (isPrime(size)) { // check if the size was a prime number if it is set cap to it
        m_currentCap = size;
    } else {
        m_currentCap = findNextPrime(size); // use this to find the next closest prime than
    }

    if (size < MINPRIME) { // size less than the min prime than set size to min prime
        m_currentCap = MINPRIME;
    }
    if(size > MAXPRIME) {
        m_currentCap = MAXPRIME;
    }

    m_currNumDeleted = 0;
    m_currentSize = 0;
    m_currentTable = new Virus[m_currentCap]; // allocate memory to the table
    for(int i = 0; i < m_currentCap; i++){ // set everything to empty
        m_currentTable[i] = EMPTY;
    }
    m_currProbing = probing;

    m_oldProbing = NONE;
    m_oldCap = 0;
    m_oldTable = nullptr;
    m_oldNumDeleted = 0;
    m_oldSize = 0;

    m_hash = hash;
    m_newPolicy = m_currProbing;
}

VDetect::~VDetect(){ // deallocate all the table
    delete [] m_currentTable;

    if (m_oldTable) {
        delete [] m_oldTable;
    }
}

void VDetect::changeProbPolicy(prob_t policy){
    if (!m_oldTable) { // set new policy if changed
        m_newPolicy = policy;
    }
}

bool VDetect::insert(Virus virus){

    if (virus.m_id < MINID || virus.m_id > MAXID) { // can't insert if this is true but still need to cqll rehash
        rehashHelper();
        return false;
    }

    if (getVirus(virus.m_key, virus.m_id) == virus) { // check for duplicates
        rehashHelper();
        return false;
    }

    insertHelper(virus); // insert your virus

    rehashHelper(); // rehash

    return true;
    // check load factor of 0.5 for insert

    // third case for rehash is if there's stuff in the old table

    // need a rehash helper function
}

bool VDetect::remove(Virus virus){
    // check for load factor of 0.8 for remove
    int index; // use for equation

    if (getVirus(virus.m_key, virus.m_id) == virus) { // the virus should exist

        if (m_currProbing == NONE) {
            index = m_hash(virus.m_key) % m_currentCap; // none equation
            if (m_currentTable[index] == virus) { // if you find it set to deleted
                m_currentTable[index] = DELETED;
                m_currNumDeleted += 1;
                rehashHelper(); // rehash
                return true;
            }
        }

        if (m_currProbing == QUADRATIC) {
            for (int i = 0; i < m_currentCap/2; ++i) {
                index = ((m_hash(virus.m_key)) % m_currentCap) + (i * i) % m_currentCap; // quadratic equation
                if (m_currentTable[index] == virus) {
                    m_currentTable[index] = DELETED;
                    m_currNumDeleted += 1;
                    rehashHelper();
                    return true;
                }
            }
        }

        if (m_currProbing == DOUBLEHASH) {
            for (int i = 0; i < m_currentCap; ++i) {
                index = ((m_hash(virus.m_key) % m_currentCap) + i * (11 - (m_hash(virus.m_key) % 11))) % m_currentCap; // double hash equation
                if (m_currentTable[index] == virus) {
                    m_currentTable[index] = DELETED;
                    m_currNumDeleted += 1;
                    rehashHelper();
                    return true;
                }
            }
        }
        // do the same for old table too
        if (m_oldProbing == NONE) {
            index = m_hash(virus.m_key) % m_oldCap;
            if (m_oldTable[index] == virus) {
                m_oldTable[index] = DELETED;
                m_oldNumDeleted += 1;
                rehashHelper();
                return true;
            }
        }

        if (m_oldProbing == QUADRATIC) {
            for (int i = 0; i < m_oldCap/2; ++i) {
                index = ((m_hash(virus.m_key)) % m_oldCap) + (i * i) % m_oldCap;
                if (m_oldTable[index] == virus) {
                    m_oldTable[index] = DELETED;
                    m_oldNumDeleted += 1;
                    rehashHelper();
                    return true;
                }
            }
        }

        if (m_oldProbing == DOUBLEHASH) {
            for (int i = 0; i < m_currentCap; ++i) {
                index = ((m_hash(virus.m_key) % m_oldCap) + i * (11 - (m_hash(virus.m_key) % 11))) % m_oldCap;
                if (m_oldTable[index] == virus) {
                    m_oldTable[index] = DELETED;
                    m_oldNumDeleted += 1;
                    rehashHelper();
                    return true;
                }
            }
        }
    }

    rehashHelper();

    return false;
}

Virus VDetect::getVirus(string key, int id) const{

    int index = 0;

    if (m_currProbing == NONE) {
        if (m_currentTable != nullptr) {
                index = m_hash(key) % m_currentCap;
                if (m_currentTable[index].m_key == key && m_currentTable[index].m_id == id) { // if it matches than it returns the virus at that index
                    return m_currentTable[index];
                }
        }
    }

    if (m_currProbing == QUADRATIC) {
        cout << "hi" << endl;
        if (m_currentTable != nullptr) {
            for (int i = 0; i < m_currentCap/2; i++) {
                index = ((m_hash(key)% m_currentCap) + (i * i)) % m_currentCap;
                cout << index << endl;
                if (m_currentTable[index].m_key == key && m_currentTable[index].m_id == id) {
                    return m_currentTable[index];
                }
            }
        }
    }

    if (m_currProbing == DOUBLEHASH) {
        if (m_currentTable != nullptr) {
            for (int i = 0; i < m_currentCap; i++) {
                index = ((m_hash(key) % m_currentCap) + i * (11 -(m_hash(key) % 11))) % m_currentCap;
                if (m_currentTable[index].m_key == key && m_currentTable[index].m_id == id) {
                    return m_currentTable[index];
                }
            }
        }
    }
    // do it for old table too
    if (m_oldProbing == NONE) {
        if (m_oldTable != nullptr) {
            index = m_hash(key) % m_oldCap;
            if (m_oldTable[index].m_key == key && m_oldTable[index].m_id == id) {
                return m_oldTable[index];
            }
        }
    }

    if (m_oldProbing == QUADRATIC) {
        if (m_oldTable != nullptr) {
            for (int i = 0; i < m_oldCap/2; i++) {
                index = ((m_hash(key)) % m_oldCap) + (i * i) % m_oldCap;
                if (m_oldTable[index].m_key == key && m_oldTable[index].m_id == id) {
                    return m_oldTable[index];
                }
            }
        }
    }

    if (m_oldProbing == DOUBLEHASH) {
        if (m_oldTable != nullptr) {
            for (int i = 0; i < m_oldCap; i++) {
                index = ((m_hash(key) % m_oldCap) + i * (11 -(m_hash(key) % 11))) % m_oldCap;
                if (m_oldTable[index].m_key == key && m_oldTable[index].m_id == id) {
                    return m_oldTable[index];
                }
            }
        }
    }

    return EMPTY;
}

float VDetect::lambda() const { // get's the load factor
    return float (m_currentSize) / float(m_currentCap);
}

float VDetect::deletedRatio() const { // get's deleted ration
    if (m_currentSize == 0) {
        return 0;
    }
    return float(m_currNumDeleted) / float (m_currentSize);
}

void VDetect::dump() const {
    cout << "Dump for the current table: " << endl;
    if (m_currentTable != nullptr)
        for (int i = 0; i < m_currentCap; i++) {
            cout << "[" << i << "] : " << m_currentTable[i] << endl;
        }
    cout << "Dump for the old table: " << endl;
    if (m_oldTable != nullptr)
        for (int i = 0; i < m_oldCap; i++) {
            cout << "[" << i << "] : " << m_oldTable[i] << endl;
        }
}

bool VDetect::isPrime(int number){
    bool result = true;
    for (int i = 2; i <= number / 2; ++i) {
        if (number % i == 0) {
            result = false;
            break;
        }
    }
    return result;
}

int VDetect::findNextPrime(int current){
    //we always stay within the range [MINPRIME-MAXPRIME]
    //the smallest prime starts at MINPRIME
    if (current < MINPRIME) current = MINPRIME-1;
    for (int i=current; i<MAXPRIME; i++) {
        for (int j=2; j*j<=i; j++) {
            if (i % j == 0)
                break;
            else if (j+1 > sqrt(i) && i != current) {
                return i;
            }
        }
    }
    //if a user tries to go over MAXPRIME
    return MAXPRIME;
}

ostream& operator<<(ostream& sout, const Virus &virus ) {
    if (!virus.m_key.empty())
        sout << virus.m_key << " (ID " << virus.m_id << ")";
    else
        sout << "";
    return sout;
}

bool operator==(const Virus& lhs, const Virus& rhs){
    return ((lhs.m_key == rhs.m_key) && (lhs.m_id == rhs.m_id));
}


void VDetect::rehashHelper() {
    if (m_oldTable == nullptr && (lambda() > 0.5 || deletedRatio() > 0.8 || m_newPolicy != m_currProbing)) { // only rehash on these condition
        m_oldProbing = m_currProbing;
        m_oldCap = m_currentCap;
        m_oldTable = m_currentTable; // set old to the cur table
        m_oldNumDeleted = m_currNumDeleted;
        m_oldSize = m_currentSize;

        m_currentCap = findNextPrime((m_currentSize - m_currNumDeleted) * 4); // set new current cap

        m_currNumDeleted = 0;
        m_currentSize = 0;

        m_currProbing = m_newPolicy;

        m_currentTable = new Virus[m_currentCap]; // make everything empty in there now
        for (int i = 0; i < m_currentCap; i++) {
            m_currentTable[i] = EMPTY;
        }
    }

    if (m_oldTable == nullptr) { // won't just rehash on an empty old table so that's why we need those other conditions
        return;
    }

    int counter = 0;

    for (int i = 0; i < m_oldCap && counter < ceil(m_oldSize / 4); i++) { //first i is to go through the whole table, counter check if to make sure to get 25% of live nodes
        if (!(m_oldTable[i] == EMPTY) && !(m_oldTable[i] == DELETED)) { // only live nodes are taken
            insertHelper(m_oldTable[i]);
            m_oldTable[i] = DELETED; // set to deleted
            m_oldNumDeleted += 1;
            counter += 1; // counter only goes up for live nodes
        }
    }

    if (m_oldNumDeleted == m_oldSize) { // the amount of deleted should equal the size as the size are the live nodes so we are done
        delete [] m_oldTable; // deallocate the old table
        m_oldTable = nullptr;
    }

}

void VDetect::insertHelper(Virus virus) { // inserting the virus into an index depending on probing
    int index;

    if (m_currProbing == NONE) {
        index = m_hash(virus.m_key) % m_currentCap;
        if (m_currentTable[index] == EMPTY || m_currentTable[index] == DELETED) { // can insert on deleted
            m_currentTable[index] = virus; // insery at index you get from hash function equation
            m_currentSize++;
        }
    }

    else if (m_currProbing == QUADRATIC) {
        for (int i = 0; i < m_currentCap/2; ++i) { // for loop for equation purposes not for indexing of current table
            index = ((m_hash(virus.m_key)) % m_currentCap) + (i * i) % m_currentCap;
            if (m_currentTable[index] == EMPTY || m_currentTable[index] == DELETED){
                m_currentTable[index] = virus;
                m_currentSize++;
                break;
                //if (lambda() > 0.5 || m_oldTable != nullptr){
                //rehash
                //}
            }
        }
    }

    else if (m_currProbing == DOUBLEHASH) {
        for (int i = 0; i < m_currentCap; ++i) {
            index = ((m_hash(virus.m_key) % m_currentCap) + i * (11 -(m_hash(virus.m_key) % 11))) % m_currentCap;
            if (m_currentTable[index] == EMPTY || m_currentTable[index] == DELETED){
                m_currentTable[index] = virus;
                m_currentSize++;
                break;
                //if (lambda() > 0.5 || m_oldTable != nullptr){
                //rehash
                //}
            }
        }
    }
    // only inserts on current table
}