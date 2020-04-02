#include <iostream>
#include <map>
#include <string>
#include <cstdlib>
#include <set>
#include <ctime>
#include <vector>

using namespace std;
string::size_type sz;

//For infection
set<long, greater <long>> infected;

//Create an adjacency list representation for the graph
map<long, set <long, greater <long>>> graph;
long totalE = 0;
long totalV = 0;
long nextV = 1;
float birthProb;
long maxIterations;


//Function for inserting an edge
void insertEdge(long u, long v){
    //cout<<"In insert edge"<<endl;
    if(u==v){
        graph.at(u).insert(v);
        totalE += 1;
    }
    else{
        graph.at(u).insert(v);
        graph.at(v).insert(u);
        totalE += 2;
    }
    //cout<<"Total vertexes "<<totalV<<" Total Edges "<<totalE<<endl;
}

//Function for deleting an edge
void deleteEdge(long u, long v){
    //cout<<"In delete edge"<<endl;
    if(u==v){
        graph.at(u).erase(v);
        totalE -= 1;
    }
    else{
        graph.at(u).erase(v);
        graph.at(v).erase(u);
        totalE -= 2;
    }
    //cout<<"Total vertexes "<<totalV<<" Total Edges "<<totalE<<endl;
}

//Function for inserting a vertex
void insertVertex(long v){
    //cout<<"In insert vertex"<<endl;
    if (graph.count(v)==1){
    }
    else{
        set <long, greater <long>> l;
        graph.insert({v, l});
        totalV++;
    }
    //cout<<"Total vertexes "<<totalV<<" Total Edges "<<totalE<<endl;
}

//Function for deleting a vertex
void deleteVertex(long v){
    //cout<<"In delete vertex"<<endl;
    if (graph.count(v)==0){    
    }
    else{
        graph.erase(v);
        totalV--;
    }
    //cout<<"Total vertexes "<<totalV<<" Total Edges "<<totalE<<endl;
}

void init(){
    long v = nextV++;
    insertVertex(v);
    insertEdge(v, v);
}

void birth(long linkNode){
    long newNode = nextV++;
    //cout<<"In Birth for node "<<linkNode<<" and "<<newNode<<endl;
    insertVertex(newNode);
    //cout<<"Inserted node "<<newNode<<endl;
    insertEdge(linkNode, newNode);
    //cout<<"Inserted edge "<<linkNode<<" and "<<newNode<<endl;
}

void death(long node){
    //cout<<"In death for node "<<node<<endl;
    set <long, greater <long>>::iterator itr;
    vector<long> list;
    for (itr = graph.at(node).begin(); itr!=graph.at(node).end(); itr++){
        //cout<<"Deleting edge "<<node<<" "<<*itr<<endl;
        list.push_back(*itr);
    }
    for (auto i = list.begin(); i != list.end(); ++i){
        deleteEdge(node, *i);
    }
    //cout<<"Deleting vertex "<<node<<endl;
    deleteVertex(node);
}

void infect(long node){
    infected.insert(node);
}

int main(int argc, char* argv[]){
    srand(time(0));
    init();
    //cout<<"Done with init"<<endl;
    birthProb = stof(argv[1]);
    maxIterations = stoi(argv[2]);
    int printInterval = stoi(argv[3]);
    int minDegree = stoi(argv[4]);
    int maxDegree = stoi(argv[5]);
    int minNodes = stoi(argv[6]);
    float spreadProb = stof(argv[7]);
    int infectIterations = stoi(argv[8]);

    //Print configuration
    cout<<"Birth Probability "<< birthProb<<endl;
    cout<<"Max iterations for building a network "<<maxIterations<<endl;
    cout<<"Network creation stat print interval "<<printInterval<<endl;
    cout<<"Min Degree for infection seeding "<<minDegree<<endl;
    cout<<"Max Degree for infection seeding "<<maxDegree<<endl;
    cout<<"Max number of nodes to be initially infected "<<minNodes<<endl;
    cout<<"Infection spread probability "<<spreadProb<<endl;
    cout<<"Number of cycles of spread "<<infectIterations<<endl;
    cout<<endl;

    for (int i=1; i<=maxIterations; i++){
        if (totalE==0){
            init();
        }
        //cout<<i<<endl;
        //Take a call for birth or death
        float call = float(rand())/float(RAND_MAX);
        //cout<<call<<" "<<birthProb<<endl;

        if (call<=birthProb){
            //Birth
            //cout<<"Getting born for call "<<call<<" with prob "<< birthProb<<endl;
            long chosenNum = rand()%(totalE); // a number between [0, totalE]
            long counter = 0;
            //cout<<"ChosenNum "<<chosenNum<<endl;

            //Iterate through all the vertexes in the graph
            map<long, set <long, greater <long>>>::iterator itr;
            long chosenNode;

            for (itr = graph.begin(); itr != graph.end(); ++itr) { 
                //Check if chosen num is in the range
                if (counter<=chosenNum && chosenNum<counter+itr->second.size()){
                    chosenNode = itr->first;
                    //cout<<"Chosen Node "<<chosenNode<<endl;
                    break;
                }
                else{
                    counter += itr->second.size();
                }
            }

            birth(chosenNode);
        }
        else{
            //Death
            //cout<<"Dying for call "<<call<<" with prob "<< birthProb<<endl;
            float chosenNum = float(rand())/float(RAND_MAX);
            float counter = 0;
            //cout<<"ChosenNum "<<chosenNum<<endl;

            //Iterate through all the vertexes in the graph
            map<long, set <long, greater <long>>>::iterator itr;
            long chosenNode = -1;  
            if((totalV*totalV - totalE)<=0){
                //cout<<"Aborting death as the totalV*totalV <= totalE "<<totalE<<endl;
                continue;   
                }
            else{
                for (itr = graph.begin(); itr != graph.end(); ++itr) {
                    //Calculate as per the fromula
                    float v = float(totalV - itr->second.size()) / float(totalV*totalV - totalE);
                    //cout<<"V Value for the node "<<v<<endl;
                    //Check if chosen num is in the range
                    if (counter<=chosenNum && chosenNum<counter+v){
                        chosenNode = itr->first;
                        //cout<<"Chosen Node "<<chosenNode<<endl;
                        break;
                    }
                    else{
                        counter += v;
                    }
                }
                if(chosenNode!=-1){
                    death(chosenNode);
                }
            }
        }

        if(i%printInterval==0){
            cout<<"Total Edges "<<totalE/2<<" Total Vertices "<<totalV<<endl;
        }
    }

    // Initialize infection with a bunch of nodes with a given degree
    map<long, set <long, greater <long>>>::iterator itr;
    int count = 0;
    for (itr = graph.begin(); itr != graph.end(); ++itr){
        if(itr->second.size()>=minDegree && itr->second.size()<=maxDegree && float(rand())/float(RAND_MAX)<=spreadProb){
            //Infect
            infect(itr->first);
            count +=1;
            if(count==minNodes){
                break;
            }
        }
    }
    cout<<"Initial infected nodes count "<<infected.size()<<endl;
    
    //Start infecting the adjacent nodes with given probability
    for (int i=0; i<infectIterations; i++){
        set<long, greater <long>> nextInfected;
        set<long, greater <long>>::iterator itr;
        //Choose a node from the infected nodes
        for (itr = infected.begin(); itr!=infected.end(); itr++){
            //cout<<"Infected "<<*itr<<endl;
            set <long, greater <long>>::iterator itr2;
            //Choose a neighbour of the infected node
            for (itr2 = graph.at(*itr).begin(); itr2!=graph.at(*itr).end(); itr2++){
                //Check if the value is not already in infected
                if (infected.find(*itr2)==infected.end() && float(rand())/float(RAND_MAX)<=spreadProb){
                    //Roll the dice and infect
                    //cout<<"Infecting "<<*itr2<<endl;
                    nextInfected.insert(*itr2);
                }
            }
        }
        for (itr = nextInfected.begin(); itr!=nextInfected.end(); itr++){
            infect(*itr);
        }
        cout<<"Current infected count "<<infected.size()<<endl;
        //cout<<infected.size()<<endl;
    }
    //Print number of infected nodes
    cout<<"Final infected nodes count "<<infected.size()<<endl;
}
