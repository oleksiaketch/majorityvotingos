// Luke Oleksiak
// CS4420
// Fall 2026
// Assignment 6

#include <iostream> // used for text input/output
#include <unistd.h> // used for for and piping
#include <sys/wait.h> // used for wait
#include <cstdlib> // used for exit and random number generation
#include <ctime> // used to see random number generator
using namespace std;

int main() 
{
    int N = -1; // initialize the number of voters to an invalid number
    int numRounds = -1; // initlize the number of rounds to an invalid number

    while(N < 3) // while an invalid number of voters
    {
        cout << "Enter the number of voters: "; // prompt the user to enter the number of voters
        cin >> N; // read in the number of voters from the user
    }
    
    while(numRounds < 1) // while an invalid number of rounds
    {
        cout << "Enter the number of rounds: "; // prompt the user to enter the number of rounds
        cin >> numRounds; // read in the number of rounds from the user
    }

    int pipes[N][2]; // create an array of pipes, enough for each voter
    for(int i=0; i<N; i++) // for loop to create the N number of pipes
    {
        pipe(pipes[i]); // for every needed pipe, create a pipe
    }


    int voterid = -1; // initializ all threads to have voter id of -1, since it's the judge voter id
    pid_t pids[N]; // creare an array of pids, one for each voter
    for(int i=0; i<N; i++) // for loop for forking N processes
    {
        pid_t currentPID = fork();
        if(currentPID > 0) // if we are in the parent process, add the PID to the list of child PIDs
        {
            pids[i] = currentPID;
        }
        else // we are in a child process, assign voter id
        {
            voterid = i;
            break; // child exits for loop
        }
    }
    //seed the random number generator
    if(voterid == -1)
        srand(time(nullptr));
    else     //if not the judge,  seed random number generator by the voter id
        srand(time(nullptr)+voterid*25);

    for(int i=1; i<=numRounds; i++)
    {
        if(voterid == -1)//if the judge,
        {
            int yes = 0; // initialize numebr of yes votes to 0
            int no = 0; // initialize number of no votes to 0
            for(int j=0; j<N; j++) // for each voter, collect the votes
            {
                char vote = 'n'; // initialize vote to n for no
                ssize_t voteread = read(pipes[j][0], &vote, sizeof(char)); // read in a vote from the pipes
                if(voteread) // if vote correctly read in from the pipes,
                {
                    if(vote == 'y')// if the vote was yes, increase the number of yes votes and print it
                    {
                        yes++;
                        cout << "Round " << i << " Voter " << j << " votes: YES" << endl;
                    }
                    else if(vote == 'n') // if the vote was no, increase the number of no votes and print it
                    {
                        no++;
                        cout << "Round " << i << " Voter " << j << " votes: NO" << endl;
                    }
                }
            } // end of vote collect loop
            // print out the results for the current round
            cout << "--- Round " << i << " the voting result is ";
            if(yes > no ) // result is yes
            {
                cout << "YES" << endl;
            } 
            else if(no > yes) // result is no
            {
                cout << "NO" << endl;
            }
            else // result is a tie
            {
                cout << "TIE" << endl;
            }
        }// end of judge case

        else // we are a voter, so cast a vote
        {
            char makevote;
            if(rand() % 2 == 0) // if an even random number is generated vote yes
            {
                makevote = 'y';
            }
            else // otherwise vote no
            {
                makevote = 'n';
            }
            // write the vote to the pipe for the judge to read
            write(pipes[voterid][1], &makevote, sizeof(char));
        } // end of voter section
    } // end of voting/judging for loop

    // the judge closes all the pipes
    if(voterid == -1)
    {
        for(int i=0; i<N; i++) // for each round
        {
            close(pipes[i][0]); // close pipe
            close(pipes[i][1]); // close pipe
        }
        // wait for all the child processes to finsih
        for(int i=0; i<N; i++)
        {
            waitpid(pids[i], nullptr, 0);
        }
    } // end of judge case
    else // voter closses all pipes
    {
        for(int i=0; i<N; i++) // for each round
        {
            close(pipes[i][0]); // close pipe
            close(pipes[i][1]);
        }
        exit(0);
    } // end of voter case
    return(0);
} // end of main