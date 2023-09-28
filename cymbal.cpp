/****************************************************************************
 *          17th Annual UCF High School Programming Tournament              *
 *                              May 2, 2003                                 *
 *                     University of Central Florida                        *
 *                                                                          *
 *                                                                          *
 * Special thanks to our sponsors: UPE, SAIC, ACM, and HARRIS               *
 *                                                                          *
 *                                                                          *
 * Problem:         Cymbal                                                  *
 * Problem Author:  Glenn                                                   *
 * Solution Author: Nick                                                    *
 * Data Author:     Phil                                                    *
 ***************************************************************************/
/******************************************************************************
* cymbal.cpp
* Judge Solution to 2003 UCF Highschool Programming Contest
* prepared by: Nick Beato
* 4/12/03
* revised for the new problem spec: 4/21/03
******************************************************************************/

/******************************************************************************
* includes
******************************************************************************/

#include <fstream>
#include <iostream>

using namespace std;

/******************************************************************************
* constants
*
* MAX_CASE: This is the maximum number of switches possible in the spec + 1
*
* NOT_CONNECTED: This is the value indicating that a switch does not directly
*				connect switch A to B.
*
* DEFAULT_CONNECTED: This indicates that two switches are connected by a track
*               by default. ie, you don't have to flip the switch and it is
*				"free" to go from switch A to switch B (or come from A to B).
*
* SWITCH_CONNECTED: This indicates that two switches can be connected directly
*				by a track.  However, switch A must be chanegd before we can
*				go onto the track leading from A to B.
*
* FORWARD_DEFAULT: A track connects the two switches AND	it is the default 
*				connection to get from A to B. This is the sum of 
*				DEFAULT_CONNECTED and SWITCH_CONNECTED to add both values to 
*				0. If it is both, then we will get this value.
*
* LARGE_NUMBER: This is just some arbitrary, but large, number. 
*               We will never have to flip more switches than this so it's the
*				best answer at the start of each test case.
*				We could also use a bool to store whether or not we have found
*				an answer, but this is easier to do (and could possibly lead
*				to a bug if LARGE_NUMBER is smaller than the answer).
*
* NOTHING_DONE: This indicates that we haven't done anything yet and primarily
*				initializes values to zero.
******************************************************************************/

const int MAX_CASE				= 101;
const int NOT_CONNECTED			= 0;
const int DEFAULT_CONNECTED		= 2;
const int SWITCH_CONNECTED		= 1;
const int FORWARD_DEFAULT		= 3;
const int LARGE_NUMBER			= 1000000;
const int NOTHING_DONE			= 0;

/******************************************************************************
* function prototypes (detailed information can be found in the instantiation)
******************************************************************************/

int get_starting(int connectivity[MAX_CASE][MAX_CASE], int num_switches);

void solve_for_one_switch(const int connectivity[MAX_CASE][MAX_CASE],
						  const int &switches_used,
						  int current_val,
						  int current_switch,
						  int last_switch,
						  int &best_so_far,
						  int answer[MAX_CASE],
						  int answer_index,
						  int best_answer[MAX_CASE]);

int connects_to_x(const int connectivity[MAX_CASE][MAX_CASE],
				  const int &switches_used,
				  const int &switch_num);


void array_copy(int to[MAX_CASE], const int from[MAX_CASE], int up_to_n);

/******************************************************************************
* main entry point
******************************************************************************/

int main()
{
  /**************************************************************************
   * Declare the file stream and open the file
   **************************************************************************/

  ifstream infile;
  infile.open("cymbal.in");

  /**************************************************************************
   * This is probably one of the harder problems to solve. So, I'm going to
   * explain the purpose of every single variable here (or try to...)
   **************************************************************************/

  /**************************************************************************
   * connectivity
   * This is an adjacency matrix.
   * It simply represents how two switches connect to each other. For the
   * purposes of this problem, it indicates how the tracks link switches.
   * (more can be found by researching "graph theory")
   * 
   * for example:
   * connectivity[2][3] will "know" how switch 2 can connect to switch 3
   *
   * connectivity[from][to] has 4 possible values: These are the global 
   * constants that I used at the beginning of this file. I'll say it
   * again to try to make my point clear.
   *
   * If connectivity[A][B] (A and B are ints) equals:
   *
   *  NOT_CONNECTED: We cannot go directly from A to B.
   *
   *  DEFAULT_CONNECTED: Switch A has a track leading to B and is ready to
   *			connect A to B without touching the switch.
   *			Note: this means it is looking "forward" or "backward"
   *
   *  SWITCH_CONNECTED: Switch A branches "forward" to B (the inverse is
   *			implied, but we don't need to store this, as we cannot go
   *			backwards on the track due to gravity.
   *
   *  FORWARD_DEFAULT: Switch A has a track leading to B and it does not
   *			need to be switched to travel to B. This is simply the sum
   *			of both other connections this represents.
   *
   * Note that the problem is acyclic, meaning you cannot return to a switch
   * that you've already visited. This is possible because the minecars are
   * moving due to gravity and we cannot move back up the track.
   * This means that we don't have to remember where we have been! yey!!
   *
   * Also note that the input specification is 1 based, not 0 based like
   * arrays are in c++/java. Instead of converting back and forth, I'm going
   * to use 1-based indexing.  So anything in row 0 or column 0 doesn't have
   * relevance to our problem! This should prevent some indexing bugs.
   **************************************************************************/

  int connectivity[MAX_CASE][MAX_CASE];

  /**************************************************************************
   * total_systems: How many systems to solve in this particular file.
   *
   * total_switches: How many switches are in the current system.
   *
   * lowest_switches: The lowest number of switches used to get to the answer
   *		so far (this could change every time or be found right away).
   *
   * cur_answer: The order that the current answer is using. We don't need to
   *		worry about a longer path because we definately should not
   *		visit any switch more than once.
   *
   * best_answer: The path of the best answer that we want to print out.
   *
   * starting_switch: This is where Skippy is!
   **************************************************************************/

  int total_systems;
  int total_switches;

  int lowest_switches;
  int cur_answer[MAX_CASE];
  int best_answer[MAX_CASE];

  int starting_switch;

  /**************************************************************************
   * Alright, now that we have all of our variables, let's start on solving
   * the problem!!!!
   *
   * First thing to do? Read in the input.
   * It makes sense to read in the input for one subproblem at a time, solving
   * each subproblem and printing it out as we go.  If anything, this will at
   * least make the output to the judges screen update and might entertain
   * them long enough to prevent "Time-Limit Exceeded"
   *
   * As specificed, the first line indicates the total number of systems. So
   * read that in, then iterate through all of the track systems.
   **************************************************************************/

  infile >> total_systems;
	
  for(int t_count = 1; t_count <= total_systems; t_count++)
  {
    /**********************************************************************
     * Before we can start solving anything, we need to initialize the 
     * answers! This is definately a good idea!
     *
     * First: set our connectivity matrix to NOT_CONNECTED
     * (we'll connect it by reading input)
     *
     * Second: set the answers to something!
     * I'm going to set the path to 0 0....
     * (remember, switch 0 is not allowed because we are 1-based)
     *
     * As for the best_number, just set it to some insanely large number.
     * Doing this will allow the worst answer to overwrite the initialized
     * values (assuming our large number is larger than the worst answer).
     **********************************************************************/

    for(int from = 0; from < MAX_CASE; from++)
      for(int to = 0; to < MAX_CASE; to++)
        connectivity[from][to] = NOT_CONNECTED;

    lowest_switches = LARGE_NUMBER;

    for(int answer_num = 0; answer_num < MAX_CASE; answer_num++)
    {
      cur_answer[answer_num] = 0;
      best_answer[answer_num] = 0;
    }

    /**********************************************************************
     * Now, we need to read in the input for one track system.
     *
     * The first line is the number of switches.
     * The next lines are the information for each of those switches
     **********************************************************************/

    infile >> total_switches;

    for(int s_count = 1; s_count <= total_switches; s_count++)
    {
			
      /******************************************************************
       * Right now we are dealing with information about switch # s_count
       *
       * It is one line: How many connections; The default switch setting;
       * then the direct connections.
       ******************************************************************/

      int how_many_connections;
      int default_setting;

      infile >> default_setting >> how_many_connections;

      /******************************************************************
       * Read in how the tracks directly connect this switch to others.
       ******************************************************************/

      for(int c_count = 1; c_count <= how_many_connections; c_count++)
      {
        int temp;
        infile >> temp;

        connectivity[s_count][temp] = SWITCH_CONNECTED;
      }

      /******************************************************************
       * Set the default!
       * I do this after reading input to avoid overwriting the setting.
       * 
       * If the switch is going "forward", then we should be setting this
       * to FORWARD_DEFAULT right now by addition.
       * If it is looking "backward", then it simply has the value of
       * DEFAULT_CONNECTED.
       ******************************************************************/

      connectivity[s_count][default_setting] += DEFAULT_CONNECTED;
    }

    /**********************************************************************
     * Yay, we finished initializing and reading the file for this case!
     * Now what can we do to help Skippy????
     *
     * I'll tell you, solve the best answer!!!
     * Now what you don't know (or maybe you do?), even if there are more
     * than one answer, we should just find an answer it and print that!
     * Think about it, he wants to get out as fast a possible.  He doesn't
     * care which "best" path he takes, he wants to know something fast!
     *
     * I'm going to use recursion to solve this, read the comments above
     * the function to understand how it works.... All you need to know by
     * looking at this line is that we are going from starting_switch to
     * the exit and storing the best answer that we get in best_answer.
     *
     * To do this, we need to send in:
     *		How everything is connected
     *		Where to start
     *		The lowest answer so far (remember: we set it to a big number)
     *		The best actual answer (so we can write to it)
     **********************************************************************/

    starting_switch = get_starting(connectivity, total_switches);

    solve_for_one_switch(connectivity, total_switches, NOTHING_DONE, 
                         starting_switch, NOTHING_DONE, lowest_switches,
                         cur_answer, NOTHING_DONE, best_answer);

    /**********************************************************************
     * Alright, now we can help Skippy get out! He would be so happy if we
     * told him the best answer now.... So let's do that!
     *
     * How, you ask? Well it's kind of complicated. We have the path stored
     * in best answer, but Skippy wants to know which switches to throw!
     * Drats! Where's the fun in that!
     *
     * Ok, we want to go through all of the switches. If you remember, I 
     * kept writing 0 0 0 0 0 0 after the path. That's helpful, because
     * now I can just keep displaying stuff until I see a 0 (again, no 
     * switch is number 0 because we are 1-based).
     *
     * I start off by setting the current switch equal to the second switch
     * and the last switch equal to the starting_switch.
     *
     * Now for every possible set of 2 switches located directly next to
     * each other (ie, current and last), we can look up if we should print
     * either one of the or both of them. How?
     *
     * If the last switch has more than one output, It is looking at the
     * current switch, so we need to print it because we could not determine
     * what to print last time around.  So we can check if it connects to
     * this switch by default. If it does, then we dont need to print (x),
     * we will need to print (x) to indicate a switch has been thrown.
     *
     * If the current switch connects many tracks to one, we need to print
     * it right away!!! We need to look at the switch setting of the last
     * switch. If it is not the default, print it with (x). Otherwise, we
     * just print the switch like normal.
     *
     * Note that once we print the value, it will not be printed as a 
     * again even though it goes through the printing conditionals twice!
     * This is because a switch can look "forward" or "backward", but it
     * can NOT be both!
     **********************************************************************/

    int path_index = 1;
    int last_switch = starting_switch;
    int current_switch;

    cout << "Track System " << t_count << ":" << endl;

    while(best_answer[path_index] != 0)
    {
      current_switch = best_answer[path_index];

      int temp_val;
			
      /******************************************************************
       * Here's the code for a switch that comes from one track and goes
       * to more than one. (A "forward" switch.)
       ******************************************************************/

      temp_val = connects_to_x(connectivity, total_switches, last_switch);
      if(temp_val > 1)
      {
        cout << last_switch;

        if( (connectivity[last_switch][current_switch] != 
             DEFAULT_CONNECTED) &&
            (connectivity[last_switch][current_switch] !=
             FORWARD_DEFAULT) )
        {
          cout << '(' << current_switch << ')';
        }

        cout << ' ';
      }

      /******************************************************************
       * Here's the code for a switch that comes from any number of tracks
       * and either leaves the system or travels along one track.
       * (A "backward" switch).
       ******************************************************************/

      temp_val = connects_to_x(connectivity, total_switches, 
                               current_switch);
      if(temp_val <= 1)
      {
        if(connectivity[current_switch][last_switch] != 
           DEFAULT_CONNECTED)
        {
          cout << '(' << last_switch << ')';
        }

        cout << current_switch;

        cout << ' ';
      }

      last_switch = current_switch;
      path_index++;
    }

    cout << endl << endl;

    /**********************************************************************
     * YAY! We solved one track system!
     **********************************************************************/
  }

  /**************************************************************************
   * Closing files is a good practice...
   **************************************************************************/

  infile.close();

  return 0;
}

/******************************************************************************
* get_starting
* We figure out the starting point by simply looking at each column
* to see if anything connects to the switch. When we find an answer, we simply
* return it, because Skippy cannot be at more than one switch right now.
******************************************************************************/

int get_starting(int connectivity[MAX_CASE][MAX_CASE], int num_switches)
{
  for(int to = 1; to <= num_switches; to++)
  {
    int counter = 0;

    for(int from = 1; from <= num_switches; from++)
    {
      if( (connectivity[from][to] == SWITCH_CONNECTED) ||
          (connectivity[from][to] == FORWARD_DEFAULT) )
        counter++;
    }

    if(counter == 0)
      return to;
  }

  /**************************************************************************
   * We should NEVER get to this line, but C++ likes to complain about missing
   * return statements, so in the bizarre case that we don't find the start
   * switch, we are gauranteed that switch 1 exists! So just return it as the
   * first one.
   **************************************************************************/

  return 1;
}

/******************************************************************************
* solve_for_one_switch
* Wow, how to explain this one??? First off: It has a lot of arguments...
*
* connectivity:   The matrix representing how the tracks connect switches.
*
* switches_used:  How many switches exist in this system.
*
* current_val:    The number of switches we've thrown so far to get here.
*
* current_switch: Which switch we are at right now.
*
* last_switch:    The last switch that we were at. This is needed for the 
*				  "backwards" to see if it is the default setting.
*
* best_so_far:    Lowest number of switches thrown in all previous solutions.
*                 Note that we need to overwrite this value as we go!
*
* answer:         The current path we are taking.
*
* answer_index:   Which index of answer we are currently able to write to.
*                 (How many switches are in the current answer.)
*
* best_answer:    The path of the best answer we've found so far.
*
* How does this work??? I really hope I can put this in english...
*
* We don't need to know how the entire system connects to solve this problem.
* All we need to know is how to get from the point A to B if A and B are
* directly connected. So that's what I'm doing. If you remember, in main, I
* call this function using STARTING_POINT as the current switch and 
* NOTHING_DONE as the last switch. So first time around, we try to go from
* STARTING_POINT to everything that STARTING_POINT can directly connect to.
* If we throw a switch, we let the system know...
*
* By doing this, we can keep track of how many switches we have to throw to
* get all the way to the exit. Because Skippy constantly moves downhill and
* there are no dead ends (it would be an exit), We will connect every switch
* that we are able to get to to everything else and eventually find the exit.
* We will keept track of the total number of switches thrown so far by simply
* adding the 0 or 1 to get from every switch on the path.
*
* example: 1 connects to 2, 3. It defaults to 2.
*
* We first say, "ok, we can get to 2 without flipping a switch, so start from
* switch 2 and see if we can get out of the track system from switch two."
*
* Likewise, we try from switch 3, but we say "we threw one more switch to get
* to 3, so remember that".
*
* The only other kicker, we have to look "backwards" as we travel on a track.
* We have to make sure that a "many-to-one" switch is defaulted to the track
* that we came from. If the last track is NOTHING_DONE, that's no biggie. But,
* that only happens on the first call, so we need to increment the switch count
* if we look "backwards" and realize that a switch was throw to get here.
*
* My last remark, by the time we get through this function, "answer" will have
* taken the value of a lot of possible paths (possibly longer than the best
* answer). I'm just saying this to help readers understand how this works.
******************************************************************************/

void solve_for_one_switch(const int connectivity[MAX_CASE][MAX_CASE],
						  const int &switches_used,
						  int current_val,
						  int current_switch,
						  int last_switch,
						  int &best_so_far,
						  int answer[MAX_CASE],
						  int answer_index,
						  int best_answer[MAX_CASE])
{
  /**************************************************************************
   * Easiest thing to do? If we are currently in a track position,
   * such that we have flipped more (or an equal number of)
   * switches then the "best" path would use, there is really no reason
   * to solve the answer from this switch to the exit, as it CAN'T be better
   * than anything previously computed.
   *
   * Essentially, if we flipped a bunch of switches, and the
   * best answer so far flipped less switches, it doesn't matter
   * where we go from here. We cannot find a better answer, so just
   * give up!
   *
   * Before we can determine that, we must look at how we got to the current
   * switch. If it's a switch with a bunch of tracks coming to it and
   * one path exiting, then we need to make sure that switch is set.
   * We count how many tracks are leaving the current switch.
   * If the number is 0, we are at the exit. If the number is 1, a whole
   * bunch came together and, after this, we are going forward to one 
   * specific switch.  In either case, we just look "backwards" to see if 
   * the switch is ok!
   **************************************************************************/

  int howmany = connects_to_x(connectivity, switches_used, current_switch);

  if((howmany <= 1) && (last_switch != NOTHING_DONE))
  {
    if(connectivity[current_switch][last_switch] == NOT_CONNECTED)
      current_val++;
  }

  /**************************************************************************
   * Here's the "give up if the current answer is worse" line.
   * This line will drastically increase timing on big test cases. If your
   * team got a lot of "Time-Limit Exceeded" responses, This is probably why!
   * If you don't believe me, remove it and run this program :)
   *
   * There are much better ways to speed this program up, but this is the
   * easiest approach and the judge data will not test for any other kind of
   * optimizations.
   **************************************************************************/

  if(current_val >= best_so_far)
    return;

  /**************************************************************************
   * Ok, if we get here, then we might be looking at a better solution.
   * So now check to see, did we exit the system?
   * If we exited then... connectivity[current_switch][1] through
   * connectivity[current_switch][switches_used] will all == NOT_CONNECTED
   *
   * If it is the best solution, we need to copy the current path
   * into our "best" path. We also need to set the number of switches thrown
   * in the best path to the current value. Once we do this, we don't need
   * to try and connect to any more switches. So, we can exit the function.
   **************************************************************************/

  if(howmany == 0)
  {
    answer[answer_index] = current_switch;

    array_copy(best_answer, answer, answer_index);

    best_so_far = current_val;

    return;
  }

  /**************************************************************************
   * Well, we didn't find the exit yet :(
   * So, let's store we're we are now in our answer and continue through the
   * program. We are going to try every connection possible and see if it is
   * the overall best answer. This can be seen as a DFS (depth first search)
   * or best visualized as having a rope and inserting it into a plumbing
   * system. The rop can go down a whole bunch of paths but only one at a
   * time. Instead of pulling it all the way out of the pipes, you try to move
   * the ope into every pipe that the current pipe you are in connects to.
   *
   * If you remember, we have 4 possibilities:
   *
   * NOT_CONNECTED: In this case, we don't need to try to find an exit
   * because we can't get there!
   *
   * SWITCH_CONNECTED: We can get there, but we must increment the current_val
   * because we have to throw a switch to use this track
   *
   * DEFAULT_CONNECTED: This setting was taken care of already, this is when
   * a track comes to a switch that yields 1 or 0 tracks. because all of the
   * tracks we want to recurse on are going "forward", this is not an option!
   *
   * FORWARD_DEFAULT: This means we can get to the next switch using a track
   * without throwing the switch we are at right now. So we just call the
   * same function changing where we are and the answer path.
   **************************************************************************/

  answer[answer_index] = current_switch;
  answer_index++;

  for(int to = 1; to <= switches_used; to++)
  {
    if(connectivity[current_switch][to] == SWITCH_CONNECTED)
    {
      if(howmany == 1)
      {
				
        solve_for_one_switch(connectivity, switches_used,
                             current_val,
                             to, current_switch, best_so_far, 
                             answer, answer_index, best_answer);
      }
      else
      {
        solve_for_one_switch(connectivity, switches_used, 
                             current_val + 1,
                             to, current_switch, best_so_far, 
                             answer, answer_index, best_answer);
      }
    }

    if(connectivity[current_switch][to] == FORWARD_DEFAULT)
    {
      solve_for_one_switch(connectivity, switches_used,
                           current_val,
                           to, current_switch, best_so_far, 
                           answer, answer_index, best_answer);
    }
  }
}


/******************************************************************************
* connects_to_x
* This function takes the connectivity matrix and which switch we are at.
* It returns how many exits the switch has as an integer.
******************************************************************************/

int connects_to_x(const int connectivity[MAX_CASE][MAX_CASE],
				  const int &switches_used,
				  const int &switch_num)
{
  int count = 0;
  for(int to = 1; to <= switches_used; to++)
    if( (connectivity[switch_num][to] == SWITCH_CONNECTED) ||
        (connectivity[switch_num][to] == FORWARD_DEFAULT) )
      count++;

  return count;
}

/******************************************************************************
* array_copy
* This copies the first n elements of "from" into "to". It then makes the
* rest of the array equal to 0.
*
* This last part should be noted because the final answer could recurse deep 
* into the matrix and change the answer. We need to make sure that we only 
* output the number of switches in the answer.
******************************************************************************/

void array_copy(int to[MAX_CASE], const int from[MAX_CASE], int up_to_n)
{
  int i;
  for(i = 0; i <= up_to_n; i++)
    to[i] = from[i];

  for(i; i < MAX_CASE; i++)
    to[i] = 0;

}


/******************************************************************************
* Well, that's it! I hope this has been insightful.
******************************************************************************/
