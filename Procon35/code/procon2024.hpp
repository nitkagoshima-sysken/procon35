#include <iostream>
#include <vector>
using namespace std;

class Board_information{
    public:
        vector<vector<int> > question_board;
        vector<vector<int> > now_board;
        void Board_show(vector<vector<int> > x);
};

class Cutting_die{
    public:
        vector<vector<int> >cutting_die_1;
        vector<vector<int> >cutting_die_2;
        vector<vector<int> >cutting_die_3;
        vector<vector<int> >cutting_die_4;
        vector<vector<int> >cutting_die_5;
        vector<vector<int> >cutting_die_6;
        vector<vector<int> >cutting_die_7;
        vector<vector<int> >cutting_die_8;
        vector<vector<int> >cutting_die_9;
        vector<vector<int> >cutting_die_10;
        vector<vector<int> >cutting_die_11;
        vector<vector<int> >cutting_die_12;
        vector<vector<int> >cutting_die_13;
        vector<vector<int> >cutting_die_14;
        vector<vector<int> >cutting_die_15;
        vector<vector<int> >cutting_die_16;
        vector<vector<int> >cutting_die_17;
        vector<vector<int> >cutting_die_18;
        vector<vector<int> >cutting_die_19;
        vector<vector<int> >cutting_die_20;
        vector<vector<int> >cutting_die_21;
        vector<vector<int> >cutting_die_22;
        vector<vector<int> >cutting_die_23;
        vector<vector<int> >cutting_die_24;
        vector<vector<int> >utting_die_25;
        void cut_and_move(vector<vector<int> > now, vector<vector<int> > Cutting_die, int direction);
};