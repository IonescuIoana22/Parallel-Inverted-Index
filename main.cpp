#include <iostream>
#include <pthread.h>
#include <vector>
#include <string>
#include <fstream>
#include <unordered_map>
#include <algorithm>

using namespace std;

struct ThreadData {
    int id;                                                 // ID-ul thread-ului
    vector<string> fileNames;                               // vector de nume de fisiere
    pthread_mutex_t *mutex;                                 // mutex prin pointer
    pthread_barrier_t *barrier;                             // bariera prin pointer
    int *idFisier;                                          // ID-ul fisierului
    vector<unordered_map<string, int>> *mapResult;          // concatenarea rezultatelor map
    int nrReduces;                                          // numarul de reduces
    int nrMaps;                                             // numarul de maps
};

string deletePunctuation(string word) {
    string result = "";
    for (int i = 0; i < word.size(); i++) {
        if ((word[i] >= 'a' && word[i] <= 'z') || (word[i] >= 'A' && word[i] <= 'Z')) {
            result += tolower(word[i]);
        }
    }
    return result;
}

unordered_map<string, int> map(string fileName, int idFisier) {
    ifstream inputFile(fileName);
    unordered_map<string, int> map;

    if (!inputFile.is_open()) {
        cout << "nu se deschide fisierul";
        exit(1);
    }

    string word;
    while (inputFile >> word) {
        word = deletePunctuation(word);
        if (word.size() != 0) {
            map[word] = idFisier + 1;
        }
    }

    inputFile.close();

    return map;
}

int sortare(pair<string, vector<int>> a, pair<string, vector<int>> b) {
    if (a.second.size() == b.second.size()) {
        return a.first < b.first;
    }
    return a.second.size() > b.second.size();
}

void reduce(vector<unordered_map<string, int>> *mapResult, int start, int end) {
    unordered_map<string, vector<int>> reduceResult;
    for (char i = 'a' + start; i < 'a' + end; i++) {
        for (auto &it : *mapResult) {
            for (auto &dict : it) {
                if (dict.first[0] == i) {
                    reduceResult[dict.first].push_back(dict.second);
                }
            }
        }
        for (auto &it : reduceResult) {
            if (it.first[0] == i) {
                sort(it.second.begin(), it.second.end());
            }
        }
        vector<pair<string, vector<int>>> toSort;
        for (auto &it : reduceResult) {
            if (it.first[0] == i) {
                toSort.push_back(it);
            }
        }
        sort(toSort.begin(), toSort.end(), sortare);
        ofstream outputFile(string(1, i) + ".txt");
        for (auto &it : toSort) {
            if (it.first[0] == i) {
                outputFile << it.first << ":[";
                for (int i = 0; i < it.second.size(); i++) {
                    if (i == it.second.size() - 1) {
                        outputFile << it.second[i];
                    } else {
                        outputFile << it.second[i] << " ";
                    }
                }
                outputFile << "]" << endl;
            }
        }
    }
}

void *f(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    int idFile = 0;
    
    while (idFile < (int)data->fileNames.size()) {
        pthread_mutex_lock(data->mutex);
        *(data->idFisier) = *(data->idFisier) + 1;
        idFile = *(data->idFisier);
        pthread_mutex_unlock(data->mutex);
        if (idFile < data->fileNames.size()) {
            (*data->mapResult)[idFile] = map(data->fileNames[idFile], idFile);
        }
    }

    pthread_barrier_wait(data->barrier);

    pthread_exit(nullptr);
}

void *g(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    int start, end;

    pthread_barrier_wait(data->barrier);

    start = (data->id - data->nrMaps) * 26 / data->nrReduces;
    if ((data->id - data->nrMaps + 1) * 26 / data->nrReduces < 26) {
        end = (data->id - data->nrMaps + 1) * 26 / data->nrReduces;
    } else {
        end = 26;
    }

    reduce(data->mapResult, start, end);

    pthread_exit(nullptr);
}

vector<string> readFileNames(const string &fileName) {
    ifstream inputFile(fileName);
    vector<string> fileNames = {};

    if (!inputFile.is_open()) {
        cout << "nu se deschide fisierul";
        return fileNames;
    }

    int n;
    inputFile >> n;

    for (int i = 0; i < n; i++) {
        string fileName;
        inputFile >> fileName;
        fileNames.push_back(fileName);
    }

    inputFile.close();
    return fileNames;
}

int main(int argc, char *argv[]) {

    if (argc < 4) {
        cout <<"ERROR\n";
        return 0;
    }

    int nrMaps = atoi(argv[1]);
    int nrReduces = atoi(argv[2]);
    string nume_fisier = argv[3];

    int idFisier = -1;

    vector<string> fileNames = readFileNames(nume_fisier);
    
    vector<unordered_map<string, int>> mapResult(fileNames.size());

    int NUM_THREADS = nrMaps + nrReduces;

    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, nullptr);

    pthread_barrier_t barrier;
    pthread_barrier_init(&barrier, nullptr, NUM_THREADS);
    
    vector<ThreadData> threadData(NUM_THREADS);
    pthread_t threads[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; i++) {
        threadData[i] = {
            i,
            fileNames,
            &mutex,
            &barrier,
            &idFisier,
            &mapResult,
            nrReduces,
            nrMaps
        };
    }
    
    for (int i = 0; i < NUM_THREADS; ++i) {
        if (i < nrMaps) {
        pthread_create(&threads[i], nullptr, f, &threadData[i]);
        } else {
            pthread_create(&threads[i], nullptr, g, &threadData[i]);
        }
    }


    
    for (int i = 0; i < NUM_THREADS; ++i) {
        pthread_join(threads[i], nullptr);
    }
    
        pthread_mutex_destroy(&mutex);
        pthread_barrier_destroy(&barrier);

    return 0; 
}
