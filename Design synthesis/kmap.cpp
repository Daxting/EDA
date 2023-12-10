#include <iostream>
#include <fstream>
#include <cstdlib>

/*This is a program for karnaugh map minimization.
  It will take kmap_in.txt as input, and output
  minimization result into kmap_out.txt. The maximum
  variable is 4.
  Input format description:
  If k=2
        A	B
        C	D
  Sequence order: A B C D
  If k=3
        A	B	C	D
        E	F	G	H
  Sequence order: A B C D E F G H
  If k=4
        A	B	C	D
        E	F	G	H
        I	J	K	L
        M	N	O	P
  Sequence order: A B C D E F G H I J K L M N O P
  For each block in the k-map, the value can be either 1 for logic 1 and 0 for logic 0.
  Following is the example of the input file:
  2
  4
  1 1 0 1 0 0 0 1 0 0 0 0 1 1 0 1
  4
  0 1 1 1 0 0 1 0 0 0 1 0 0 0 1 0
  In this example, the k-map with 4 variables (w,x,y,z) is as follows:
  1	1	0	1
  0	0	0	1
  0	0	0	0
  1	1	0	1

  Output format description:
  In the output file, A Boolean function is represented in the SOP form.
  he minimized Boolean function as x’z’+x’y’+w’yz’ and can be represented in the first 4 lines of the output file as follows:
  #1
  2 0 2 0
  2 0 0 2
  0 2 1 0
  #2
  …
  …
  0 means logic 0, 1 meas logic 1 and 2 means don't care.*/


using namespace std;

void top()
{
    ifstream file ("kmap_in.txt",ios::in);
    int Case;
    int variables;
    int counter=1;

    file >> Case ;

    ofstream out ("kmap_out.txt",ios::out);

    while( file && Case>0 )
    {
        //cout << "#" << counter << endl ;
        out << "#" << counter << endl ;
        counter++;
        int judge=0;
        int c=0;
        int output[4]={2,2,2,2};
        int kmap[6][6]={};
        file >> variables ;
        if(variables==2)
        {
            for(int i=1;i<3;i++)
            {
                for(int k=1;k<3;k++)
                {
                    int value ;
                    file >> value ;
                    kmap[i][k]=value;
                    if(k==1)
                        kmap[i][k+2]=kmap[i][k];
                    if(k==2)
                        kmap[i][k-2]=kmap[i][k];
                    if(i==1)
                        kmap[i+2][k]=kmap[i][k];
                    if(i==2)
                        kmap[i-2][k]=kmap[i][k];
                }
            }

        // read inputs

            for(int i=1;i<3;i++)
            {
                for(int k=1;k<3;k++)
                {
                    if(kmap[i][k]==0)
                    {
                        c=1;
                        judge=1;
                        break;
                    }
                }
                if(c==1)
                    break;
            }
            if(judge==0)
            {
                for(int i=1;i<3;i++)
                {
                    for(int k=1;k<3;k++)
                        kmap[i][k]++;
                }
                for(int i=0 ;i<2;i++)
                {
                    out << output[i] << " " ;
                    //cout << output[i] << " " ;
                }
                out << endl;
                //cout<<endl;
                //out << endl;
                for(int i=0 ;i<2;i++)
                    output[i]=2;
            }

    // if all elements are 1

            for(int a=1;a<3;a++)
            {
                for(int b=1;b<3;b++)
                {
                    if(kmap[a][b]==1)
                    {
                        if(kmap[a-1][b]!=0)
                        {
                            if((a-1)==0)
                                kmap[2][b]++;

                            kmap[a-1][b]++;
                            kmap[a][b]++;

                            if(b==1)
                            output[1]=0;
                            if(b==2)
                            output[1]=1;

                            for(int i=0 ;i<2;i++)
                            {
                                out << output[i] << " " ;
                                //cout << output[i] << " " ;
                            }
                            out << endl;
                            //cout<<endl;
                            //out << endl;
                            for(int i=0 ;i<2;i++)
                                output[i]=2;
                        }
                        else if(kmap[a][b-1]!=0)
                        {
                            if((b-1)==0)
                                kmap[a][2]++;

                            kmap[a][b-1]++;
                            kmap[a][b]++;

                            if(a==1)
                                output[0]=0;
                            if(a==2)
                                output[0]=1;

                            for(int i=0 ;i<2;i++)
                            {
                                out << output[i] << " " ;
                                //cout << output[i] << " " ;
                            }
                            out << endl;
                            //cout<<endl;
                            //out << endl;
                            for(int i=0 ;i<2;i++)
                                output[i]=2;
                        }

                // enclose 2 elements

                        else
                        {
                            if(a==1)
                                output[0]=0;
                            if(a==2)
                                output[0]=1;
                            if(b==1)
                                output[1]=0;
                            if(b==2)
                                output[1]=1;

                            for(int i=0 ;i<2;i++)
                            {
                                out << output[i] << " " ;
                                //cout << output[i] << " " ;
                            }
                            out << endl;
                            //cout<<endl;
                            //out << endl;
                            for(int i=0 ;i<2;i++)
                                output[i]=2;
                        }

                // no elements can be enclosed
                    }
                }
            }
            judge=0;
            c=0;
        } // end if 2 variables


        if(variables==3)
        {
            for(int i=1;i<3;i++)
            {
                for(int k=1;k<5;k++)
                {
                    int value ;
                    file >> value ;
                    kmap[i][k]=value;
                    if(k==1)
                        kmap[i][k+4]=kmap[i][k];
                    if(k==4)
                        kmap[i][k-4]=kmap[i][k];
                    if(i==1)
                        kmap[i+2][k]=kmap[i][k];
                    if(i==2)
                        kmap[i-2][k]=kmap[i][k];
                }
            }

        // read inputs

            for(int i=1;i<3;i++)
            {
                for(int k=1;k<5;k++)
                {
                    if(kmap[i][k]==0)
                    {
                        c=1;
                        judge=1;
                        break;
                    }
                }
                if(c==1)
                    break;
            }
            if(judge==0)
            {
                for(int i=1;i<3;i++)
                {
                    for(int k=1;k<5;k++)
                        kmap[i][k]++;
                }
                for(int i=0 ;i<3;i++)
                {
                    out << output[i] << " " ;
                    //cout << output[i] << " " ;
                }
                out << endl;
                //cout<<endl;
                //out << endl;
                for(int i=0 ;i<3;i++)
                    output[i]=2;
            }

    // if all elements are 1

            for(int a=1;a<3;a++)
            {
                for(int b=1;b<5;b++)
                {
                    if(kmap[a][b]==1)
                    {
                        if(kmap[a][b]!=0&&kmap[a][b+1]!=0&&kmap[a][b+2]!=0&&kmap[a][b+3]!=0&&b==1)
                        {
                            kmap[a][b]++;
                            kmap[a][b+1]++;
                            kmap[a][b+2]++;
                            kmap[a][b+3]++;

                            if(a==1)
                                output[0]=0;
                            if(a==2)
                                output[0]=1;

                            for(int i=0 ;i<3;i++)
                            {
                                out << output[i] << " " ;
                                //cout << output[i] << " " ;
                            }
                            out << endl;
                            //cout<<endl;
                            //out << endl;
                            for(int i=0 ;i<3;i++)
                                output[i]=2;
                        }

                // enclose 4 straight elements

                        else if(kmap[a-1][b]!=0 && kmap[a][b-1]!=0 && kmap[a-1][b-1]!=0)
                        {
                            if((a-1)==0)
                                kmap[2][b]++;
                            if((b-1)==0)
                                kmap[a][4]++;

                            kmap[a][b]++;
                            kmap[a-1][b]++;
                            kmap[a][b-1]++;
                            kmap[a-1][b-1]++;

                            if(b==1)
                                output[2]=0;
                            if(b==2)
                                output[1]=0;
                            if(b==3)
                                output[2]=1;
                            if(b==4)
                                output[1]=1;

                            for(int i=0 ;i<3;i++)
                            {
                                out << output[i] << " " ;
                                //cout << output[i] << " " ;
                            }
                            out << endl;
                            //cout<<endl;
                            //out << endl;
                            for(int i=0 ;i<3;i++)
                                output[i]=2;
                        }
                        else if(kmap[a+1][b]!=0 && kmap[a][b+1]!=0 && kmap[a+1][b+1]!=0)
                        {
                            if((a+1)==3)
                                kmap[1][b]++;
                            if((b+1)==5)
                                kmap[a][1]++;

                            kmap[a][b]++;
                            kmap[a+1][b]++;
                            kmap[a][b+1]++;
                            kmap[a+1][b+1]++;

                            if(b==1)
                                output[1]=0;
                            if(b==2)
                                output[2]=1;
                            if(b==3)
                                output[1]=1;
                            if(b==4)
                                output[2]=0;

                            for(int i=0 ;i<3;i++)
                            {
                                out << output[i] << " " ;
                                //cout << output[i] << " " ;
                            }
                            out << endl;
                            //cout<<endl;
                            //out << endl;
                            for(int i=0 ;i<3;i++)
                                output[i]=2;
                        }

                // enclose 4 square elements

                        else if(kmap[a-1][b]!=0)
                        {

                            if((a-1)==0)
                                kmap[2][b]++;

                            kmap[a-1][b]++;
                            kmap[a][b]++;

                            if(b==1)
                            {
                                output[1]=0;
                                output[2]=0;
                            }
                            if(b==2)
                            {
                                output[1]=0;
                                output[2]=1;
                            }
                            if(b==3)
                            {
                                output[1]=1;
                                output[2]=1;
                            }
                            if(b==4)
                            {
                                output[1]=1;
                                output[2]=0;
                            }

                            for(int i=0 ;i<3;i++)
                            {
                                out << output[i] << " " ;
                                //cout << output[i] << " " ;
                            }
                            out << endl;
                            //cout<<endl;
                            //out << endl;
                            for(int i=0 ;i<3;i++)
                                output[i]=2;
                        }
                        else if(kmap[a][b-1]!=0)
                        {
                            if((b-1)==0)
                                kmap[a][4]++;

                            kmap[a][b-1]++;
                            kmap[a][b]++;

                            if(a==1)
                                output[0]=0;
                            if(a==2)
                                output[0]=1;
                            if(b==1)
                                output[2]=0;
                            if(b==2)
                                output[1]=0;
                            if(b==3)
                                output[2]=1;
                            if(b==4)
                                output[1]=1;

                            for(int i=0 ;i<3;i++)
                            {
                                out << output[i] << " " ;
                                //cout << output[i] << " " ;
                            }
                            out << endl;
                            //cout<<endl;
                            //out << endl;
                            for(int i=0 ;i<3;i++)
                                output[i]=2;
                        }
                        else if(kmap[a][b+1]!=0)
                        {

                            if((b+1)==5)
                                kmap[a][1]++;

                            kmap[a][b+1]++;
                            kmap[a][b]++;

                            if(a==1)
                            output[0]=0;
                            if(a==2)
                            output[0]=1;
                            if(b==1)
                                output[1]=0;
                            if(b==2)
                                output[2]=1;
                            if(b==3)
                                output[1]=1;
                            if(b==4)
                                output[2]=0;

                            for(int i=0 ;i<3;i++)
                            {
                                out << output[i] << " " ;
                                //cout << output[i] << " " ;
                            }
                            out << endl;
                            //cout<<endl;
                            //out << endl;
                            for(int i=0 ;i<3;i++)
                                output[i]=2;
                        }

                // enclose 2 elements

                        else
                        {

                            if(a==1)
                                output[0]=0;
                            if(a==2)
                                output[0]=1;
                            if(b==1)
                            {
                                output[1]=0;
                                output[2]=0;
                            }
                            if(b==2)
                            {
                                output[1]=0;
                                output[2]=1;
                            }
                            if(b==3)
                            {
                                output[1]=1;
                                output[2]=1;
                            }
                            if(b==4)
                            {
                                output[1]=1;
                                output[2]=0;
                            }

                            for(int i=0 ;i<3;i++)
                            {
                                out << output[i] << " " ;
                                //cout << output[i] << " " ;
                            }
                            out << endl;
                            //cout<<endl;
                            //out << endl;
                            for(int i=0 ;i<3;i++)
                                output[i]=2;
                        }

                // no elements can be enclosed

                    }
                }
            }
            judge=0;
            c=0;
        }// end if 3 variables

        if(variables==4)
        {
            for(int i=1;i<5;i++)
            {
                for(int k=1;k<5;k++)
                {
                    int value ;
                    file >> value ;
                    kmap[i][k]=value;
                    if(k==1)
                        kmap[i][k+4]=kmap[i][k];
                    if(k==4)
                        kmap[i][k-4]=kmap[i][k];
                    if(i==1)
                        kmap[i+4][k]=kmap[i][k];
                    if(i==4)
                        kmap[i-4][k]=kmap[i][k];
                }
            }
         // read inputs

         for(int i=1;i<5;i++)
            {
                for(int k=1;k<5;k++)
                {
                    if(kmap[i][k]==0)
                    {
                        c=1;
                        judge=1;
                        break;
                    }
                }
                if(c==1)
                    break;
            }
            if(judge==0)
            {
                for(int i=1;i<5;i++)
                {
                    for(int k=1;k<5;k++)
                        kmap[i][k]++;
                }
                for(int i=0 ;i<4;i++)
                {
                    out << output[i] << " " ;
                    //cout << output[i] << " " ;
                }
                out << endl;
                //cout<<endl;
                //out << endl;
                for(int i=0 ;i<3;i++)
                output[i]=2;
            }

    // if all elements are 1

            for(int a=1;a<5;a++)
            {
                for(int b=1;b<5;b++)
                {
                    if(kmap[a][b]==1)
                    {
                        if((b==1&&kmap[a][b+1]!=0&&kmap[a][b+2]!=0&&kmap[a][b+3]!=0&&kmap[a-1][b]!=0&&kmap[a-1][b+1]!=0&&kmap[a-1][b+2]!=0&&kmap[a-1][b+3]!=0)||
                           (b==2&&kmap[a][b+1]!=0&&kmap[a][b+2]!=0&&kmap[a][b-1]!=0&&kmap[a-1][b]!=0&&kmap[a-1][b+1]!=0&&kmap[a-1][b+2]!=0&&kmap[a-1][b-1]!=0)||
                           (b==3&&kmap[a][b+1]!=0&&kmap[a][b-1]!=0&&kmap[a][b-2]!=0&&kmap[a-1][b]!=0&&kmap[a-1][b+1]!=0&&kmap[a-1][b-1]!=0&&kmap[a-1][b-2]!=0)||
                           (b==4&&kmap[a][b-1]!=0&&kmap[a][b-2]!=0&&kmap[a][b-3]!=0&&kmap[a-1][b]!=0&&kmap[a-1][b-1]!=0&&kmap[a-1][b-2]!=0&&kmap[a-1][b-3]!=0))
                        {
                            if((a-1)==0)
                            {
                                kmap[4][1]++;
                                kmap[4][2]++;
                                kmap[4][3]++;
                                kmap[4][4]++;
                            }

                            kmap[a][1]++;
                            kmap[a][2]++;
                            kmap[a][3]++;
                            kmap[a][4]++;
                            kmap[a-1][1]++;
                            kmap[a-1][2]++;
                            kmap[a-1][3]++;
                            kmap[a-1][4]++;

                            if(a==1)
                                output[1]=0;
                            if(a==2)
                                output[0]=0;
                            if(a==3)
                                output[1]=1;
                            if(a==4)
                                output[0]=1;

                            for(int i=0 ;i<4;i++)
                            {
                                out << output[i] << " " ;
                                //cout << output[i] << " " ;
                            }
                            out << endl;
                            //cout<<endl;
                            //out << endl;
                            for(int i=0 ;i<4;i++)
                                output[i]=2;
                        }
                        else if((b==1&&kmap[a][b+1]!=0&&kmap[a][b+2]!=0&&kmap[a][b+3]!=0&&kmap[a+1][b]!=0&&kmap[a+1][b+1]!=0&&kmap[a+1][b+2]!=0&&kmap[a+1][b+3]!=0)||
                                (b==2&&kmap[a][b+1]!=0&&kmap[a][b+2]!=0&&kmap[a][b-1]!=0&&kmap[a+1][b]!=0&&kmap[a+1][b+1]!=0&&kmap[a+1][b+2]!=0&&kmap[a+1][b-1]!=0)||
                                (b==3&&kmap[a][b+1]!=0&&kmap[a][b-1]!=0&&kmap[a][b-2]!=0&&kmap[a+1][b]!=0&&kmap[a+1][b+1]!=0&&kmap[a+1][b-1]!=0&&kmap[a+1][b-2]!=0)||
                                (b==4&&kmap[a][b-1]!=0&&kmap[a][b-2]!=0&&kmap[a][b-3]!=0&&kmap[a+1][b]!=0&&kmap[a+1][b-1]!=0&&kmap[a+1][b-2]!=0&&kmap[a+1][b-3]!=0))
                        {
                            if((a+1)==5)
                            {
                                kmap[1][1]++;
                                kmap[1][2]++;
                                kmap[1][3]++;
                                kmap[1][4]++;
                            }

                            kmap[a][1]++;
                            kmap[a][2]++;
                            kmap[a][3]++;
                            kmap[a][4]++;
                            kmap[a+1][1]++;
                            kmap[a+1][2]++;
                            kmap[a+1][3]++;
                            kmap[a+1][4]++;

                            if(a==1)
                                output[0]=0;
                            if(a==2)
                                output[1]=1;
                            if(a==3)
                                output[0]=1;
                            if(a==4)
                                output[1]=0;

                            for(int i=0 ;i<4;i++)
                            {
                                out << output[i] << " " ;
                                //cout << output[i] << " " ;
                            }
                            out << endl;
                            //cout<<endl;
                            //out << endl;
                            for(int i=0 ;i<4;i++)
                                output[i]=2;
                        }
                        else if((a==1&&kmap[a+1][b]!=0&&kmap[a+2][b]!=0&&kmap[a+3][b]!=0&&kmap[a][b-1]!=0&&kmap[a+1][b-1]!=0&&kmap[a+2][b-1]!=0&&kmap[a+3][b-1]!=0)||
                                (a==2&&kmap[a+1][b]!=0&&kmap[a+2][b]!=0&&kmap[a-1][b]!=0&&kmap[a][b-1]!=0&&kmap[a+1][b-1]!=0&&kmap[a+2][b-1]!=0&&kmap[a-1][b-1]!=0)||
                                (a==3&&kmap[a+1][b]!=0&&kmap[a-1][b]!=0&&kmap[a-2][b]!=0&&kmap[a][b-1]!=0&&kmap[a+1][b-1]!=0&&kmap[a-1][b-1]!=0&&kmap[a-2][b-1]!=0)||
                                (a==4&&kmap[a-1][b]!=0&&kmap[a-2][b]!=0&&kmap[a-3][b]!=0&&kmap[a][b-1]!=0&&kmap[a-1][b-1]!=0&&kmap[a-2][b-1]!=0&&kmap[a-3][b-1]!=0))
                        {
                            if((b-1)==0)
                            {
                                kmap[1][4]++;
                                kmap[2][4]++;
                                kmap[3][4]++;
                                kmap[4][4]++;
                            }

                            kmap[1][b]++;
                            kmap[2][b]++;
                            kmap[3][b]++;
                            kmap[4][b]++;
                            kmap[1][b-1]++;
                            kmap[2][b-1]++;
                            kmap[3][b-1]++;
                            kmap[4][b-1]++;

                            if(b==1)
                                output[3]=0;
                            if(b==2)
                                output[2]=0;
                            if(b==3)
                                output[3]=1;
                            if(b==4)
                                output[2]=1;

                            for(int i=0 ;i<4;i++)
                            {
                                out << output[i] << " " ;
                                //cout << output[i] << " " ;
                            }
                            out << endl;
                            //cout<<endl;
                            //out << endl;
                            for(int i=0 ;i<4;i++)
                                output[i]=2;
                        }
                        else if((a==1&&kmap[a+1][b]!=0&&kmap[a+2][b]!=0&&kmap[a+3][b]!=0&&kmap[a][b+1]!=0&&kmap[a+1][b+1]!=0&&kmap[a+2][b+1]!=0&&kmap[a+3][b+1]!=0)||
                                (a==2&&kmap[a+1][b]!=0&&kmap[a+2][b]!=0&&kmap[a-1][b]!=0&&kmap[a][b+1]!=0&&kmap[a+1][b+1]!=0&&kmap[a+2][b+1]!=0&&kmap[a-1][b+1]!=0)||
                                (a==3&&kmap[a+1][b]!=0&&kmap[a-1][b]!=0&&kmap[a-2][b]!=0&&kmap[a][b+1]!=0&&kmap[a+1][b+1]!=0&&kmap[a-1][b+1]!=0&&kmap[a-2][b+1]!=0)||
                                (a==4&&kmap[a-1][b]!=0&&kmap[a-2][b]!=0&&kmap[a-3][b]!=0&&kmap[a][b+1]!=0&&kmap[a-1][b+1]!=0&&kmap[a-2][b+1]!=0&&kmap[a-3][b+1]!=0))
                        {
                            if((b+1)==5)
                            {
                                kmap[1][1]++;
                                kmap[2][1]++;
                                kmap[3][1]++;
                                kmap[4][1]++;
                            }

                            kmap[1][b]++;
                            kmap[2][b]++;
                            kmap[3][b]++;
                            kmap[4][b]++;
                            kmap[1][b+1]++;
                            kmap[2][b+1]++;
                            kmap[3][b+1]++;
                            kmap[4][b+1]++;

                            if(b==1)
                                output[2]=0;
                            if(b==2)
                                output[3]=1;
                            if(b==3)
                                output[2]=1;
                            if(b==4)
                                output[3]=0;

                            for(int i=0 ;i<4;i++)
                            {
                                out << output[i] << " " ;
                                //cout << output[i] << " " ;
                            }
                            out << endl;
                            //cout<<endl;
                            //out << endl;
                            for(int i=0 ;i<4;i++)
                                output[i]=2;
                        }

                // enclose 8 rectangular elements


                        else if((a==1&&b==1&&kmap[1][4]!=0&&kmap[4][1]!=0&&kmap[4][4]!=0)||
                                (a==1&&b==4&&kmap[1][1]!=0&&kmap[4][1]!=0&&kmap[4][4]!=0)||
                                (a==4&&b==1&&kmap[1][4]!=0&&kmap[1][1]!=0&&kmap[4][4]!=0)||
                                (a==4&&b==4&&kmap[1][4]!=0&&kmap[4][1]!=0&&kmap[1][1]!=0))
                        {
                            kmap[1][1]++;
                            kmap[1][4]++;
                            kmap[4][1]++;
                            kmap[4][4]++;
                            output[1]=0;
                            output[3]=0;

                            for(int i=0 ;i<4;i++)
                            {
                                out << output[i] << " " ;
                                //cout << output[i] << " " ;
                            }
                            out << endl;
                            //cout<<endl;
                            //out << endl;
                            for(int i=0 ;i<4;i++)
                                output[i]=2;
                        }

                // enclose 4 corner elements

                        else if((b==1&&kmap[a][b]!=0&&kmap[a][b+1]!=0&&kmap[a][b+2]!=0&&kmap[a][b+3]!=0)||
                                (b==2&&kmap[a][b]!=0&&kmap[a][b+1]!=0&&kmap[a][b+2]!=0&&kmap[a][b-1]!=0)||
                                (b==3&&kmap[a][b]!=0&&kmap[a][b+1]!=0&&kmap[a][b-1]!=0&&kmap[a][b-2]!=0)||
                                (b==4&&kmap[a][b]!=0&&kmap[a][b-1]!=0&&kmap[a][b-2]!=0&&kmap[a][b-3]!=0))
                        {
                            kmap[a][1]++;
                            kmap[a][2]++;
                            kmap[a][3]++;
                            kmap[a][4]++;

                            if(a==1)
                            {
                                output[0]=0;
                                output[1]=0;
                            }
                            if(a==2)
                            {
                                output[0]=0;
                                output[1]=1;
                            }
                            if(a==3)
                            {
                                output[0]=1;
                                output[1]=1;
                            }
                            if(a==4)
                            {
                                output[0]=1;
                                output[1]=0;
                            }

                            for(int i=0 ;i<4;i++)
                            {
                                out << output[i] << " " ;
                                //cout << output[i] << " " ;
                            }
                            out << endl;
                            //cout<<endl;
                            //out << endl;
                            for(int i=0 ;i<4;i++)
                                output[i]=2;
                        }
                        else if((a==1&&kmap[a][b]!=0&&kmap[a+1][b]!=0&&kmap[a+2][b]!=0&&kmap[a+3][b]!=0)||
                                (a==2&&kmap[a][b]!=0&&kmap[a+1][b]!=0&&kmap[a+2][b]!=0&&kmap[a-1][b]!=0)||
                                (a==3&&kmap[a][b]!=0&&kmap[a+1][b]!=0&&kmap[a-1][b]!=0&&kmap[a-2][b]!=0)||
                                (a==4&&kmap[a][b]!=0&&kmap[a-1][b]!=0&&kmap[a-2][b]!=0&&kmap[a-3][b]!=0))
                        {
                            kmap[1][b]++;
                            kmap[2][b]++;
                            kmap[3][b]++;
                            kmap[4][b]++;

                            if(b==1)
                            {
                                output[2]=0;
                                output[3]=0;
                            }
                            if(b==2)
                            {
                                output[2]=0;
                                output[3]=1;
                            }
                            if(b==3)
                            {
                                output[2]=1;
                                output[3]=1;
                            }
                            if(b==4)
                            {
                                output[2]=1;
                                output[3]=0;
                            }
                            for(int i=0 ;i<4;i++)
                            {
                                out << output[i] << " " ;
                                //cout << output[i] << " " ;
                            }
                            out << endl;
                            //cout<<endl;
                            //out << endl;
                            for(int i=0 ;i<4;i++)
                                output[i]=2;
                        }

                // enclose 4 straight elements

                        else if(kmap[a-1][b]!=0 && kmap[a][b-1]!=0 && kmap[a-1][b-1]!=0)
                        {
                            if((a-1)==0)
                                kmap[4][b]++;
                            if((b-1)==0)
                                kmap[a][4]++;

                            kmap[a][b]++;
                            kmap[a-1][b]++;
                            kmap[a][b-1]++;
                            kmap[a-1][b-1]++;

                            if(a==1)
                                output[1]=0;
                            if(a==2)
                                output[0]=0;
                            if(a==3)
                                output[1]=1;
                            if(a==4)
                                output[0]=1;
                            if(b==1)
                                output[3]=0;
                            if(b==2)
                                output[2]=0;
                            if(b==3)
                                output[3]=1;
                            if(b==4)
                                output[2]=1;

                            for(int i=0 ;i<4;i++)
                            {
                                out << output[i] << " " ;
                                //cout << output[i] << " " ;
                            }
                            out << endl;
                            //cout<<endl;
                            //out << endl;
                            for(int i=0 ;i<4;i++)
                                output[i]=2;
                        }
                        else if(kmap[a+1][b]!=0 && kmap[a][b-1]!=0 && kmap[a+1][b-1]!=0)
                        {
                            if((a+1)==5)
                                kmap[1][b]++;
                            if((b-1)==0)
                                kmap[a][4]++;

                            kmap[a][b]++;
                            kmap[a+1][b]++;
                            kmap[a][b-1]++;
                            kmap[a+1][b-1]++;

                            if(a==1)
                                output[0]=0;
                            if(a==2)
                                output[1]=1;
                            if(a==3)
                                output[0]=1;
                            if(a==4)
                                output[1]=0;
                            if(b==1)
                                output[3]=0;
                            if(b==2)
                                output[2]=0;
                            if(b==3)
                                output[3]=1;
                            if(b==4)
                                output[2]=1;

                            for(int i=0 ;i<4;i++)
                            {
                                out << output[i] << " " ;
                                //cout << output[i] << " " ;
                            }
                            out << endl;
                            //cout<<endl;
                            //out << endl;
                            for(int i=0 ;i<4;i++)
                                output[i]=2;
                        }
                        else if(kmap[a+1][b]!=0 && kmap[a][b+1]!=0 && kmap[a+1][b+1]!=0)
                        {
                            if((a+1)==5)
                                kmap[1][b]++;
                            if((b+1)==5)
                                kmap[a][1]++;

                            kmap[a][b]++;
                            kmap[a+1][b]++;
                            kmap[a][b+1]++;
                            kmap[a+1][b+1]++;

                            if(a==1)
                                output[0]=0;
                            if(a==2)
                                output[1]=1;
                            if(a==3)
                                output[0]=1;
                            if(a==4)
                                output[1]=0;
                            if(b==1)
                            output[2]=0;
                            if(b==2)
                            output[3]=1;
                            if(b==3)
                            output[2]=1;
                            if(b==4)
                            output[3]=0;

                            for(int i=0 ;i<4;i++)
                            {
                                out << output[i] << " " ;
                                //cout << output[i] << " " ;
                            }
                            out << endl;
                            //cout<<endl;
                            //out << endl;
                            for(int i=0 ;i<4;i++)
                                output[i]=2;
                        }
                        else if(kmap[a-1][b]!=0 && kmap[a][b+1]!=0 && kmap[a-1][b+1]!=0)
                        {
                            if((a-1)==0)
                                kmap[4][b]++;
                            if((b+1)==5)
                                kmap[a][1]++;

                            kmap[a][b]++;
                            kmap[a-1][b]++;
                            kmap[a][b+1]++;
                            kmap[a-1][b+1]++;

                            if(a==1)
                                output[1]=0;
                            if(a==2)
                                output[0]=0;
                            if(a==3)
                                output[1]=1;
                            if(a==4)
                                output[0]=1;
                            if(b==1)
                            output[2]=0;
                            if(b==2)
                            output[3]=1;
                            if(b==3)
                            output[2]=1;
                            if(b==4)
                            output[3]=0;

                            for(int i=0 ;i<4;i++)
                            {
                                out << output[i] << " " ;
                                //cout << output[i] << " " ;
                            }
                            out << endl;
                            //cout<<endl;
                            //out << endl;
                            for(int i=0 ;i<4;i++)
                                output[i]=2;
                        }

                // enclose 4 square elements

                        else if(kmap[a-1][b]!=0)
                        {

                            if((a-1)==0)
                                kmap[4][b]++;

                            kmap[a-1][b]++;
                            kmap[a][b]++;

                            if(a==1)
                                output[1]=0;
                            if(a==2)
                                output[0]=0;
                            if(a==3)
                                output[1]=1;
                            if(a==4)
                                output[0]=1;
                            if(b==1)
                            {
                                output[2]=0;
                                output[3]=0;
                            }
                            if(b==2)
                            {
                                output[2]=0;
                                output[3]=1;
                            }
                            if(b==3)
                            {
                                output[2]=1;
                                output[3]=1;
                            }
                            if(b==4)
                            {
                                output[2]=1;
                                output[3]=0;
                            }

                            for(int i=0 ;i<4;i++)
                            {
                                out << output[i] << " " ;
                                //cout << output[i] << " " ;
                            }
                            out << endl;
                            //cout<<endl;
                            //out << endl;
                            for(int i=0 ;i<4;i++)
                                output[i]=2;
                        }
                        else if(kmap[a+1][b]!=0)
                        {
                            if((a+1)==0)
                                kmap[1][b]++;

                            kmap[a+1][b]++;
                            kmap[a][b]++;

                            if(a==1)
                                output[0]=0;
                            if(a==2)
                                output[1]=1;
                            if(a==3)
                                output[0]=1;
                            if(a==4)
                                output[1]=0;
                            if(b==1)
                            {
                                output[2]=0;
                                output[3]=0;
                            }
                            if(b==2)
                            {
                                output[2]=0;
                                output[3]=1;
                            }
                            if(b==3)
                            {
                                output[2]=1;
                                output[3]=1;
                            }
                            if(b==4)
                            {
                                output[2]=1;
                                output[3]=0;
                            }

                            for(int i=0 ;i<4;i++)
                            {
                                out << output[i] << " " ;
                                //cout << output[i] << " " ;
                            }
                            out << endl;
                            //cout<<endl;
                            //out << endl;
                            for(int i=0 ;i<4;i++)
                                output[i]=2;
                        }
                        else if(kmap[a][b-1]!=0)
                        {
                            if((b-1)==0)
                                kmap[a][4]++;

                            kmap[a][b-1]++;
                            kmap[a][b]++;

                            if(a==1)
                            {
                                output[0]=0;
                                output[1]=0;
                            }
                            if(a==2)
                            {
                                output[0]=0;
                                output[1]=1;
                            }
                            if(a==3)
                            {
                                output[0]=1;
                                output[1]=1;
                            }
                            if(a==4)
                            {
                                output[0]=1;
                                output[1]=0;
                            }
                            if(b==1)
                                output[3]=0;
                            if(b==2)
                                output[2]=0;
                            if(b==3)
                                output[3]=1;
                            if(b==4)
                                output[2]=1;

                            for(int i=0 ;i<4;i++)
                            {
                                out << output[i] << " " ;
                                //cout << output[i] << " " ;
                            }
                            out << endl;
                            //cout<<endl;
                            //out << endl;
                            for(int i=0 ;i<4;i++)
                                output[i]=2;
                        }
                        else if(kmap[a][b+1]!=0)
                        {
                            if((b+1)==5)
                                kmap[a][1]++;

                            kmap[a][b+1]++;
                            kmap[a][b]++;

                            if(a==1)
                            {
                                output[0]=0;
                                output[1]=0;
                            }
                            if(a==2)
                            {
                                output[0]=0;
                                output[1]=1;
                            }
                            if(a==3)
                            {
                                output[0]=1;
                                output[1]=1;
                            }
                            if(a==4)
                            {
                                output[0]=1;
                                output[1]=0;
                            }
                            if(b==1)
                                output[2]=0;
                            if(b==2)
                                output[3]=1;
                            if(b==3)
                                output[2]=1;
                            if(b==4)
                                output[3]=0;

                            for(int i=0 ;i<4;i++)
                            {
                                out << output[i] << " " ;
                                //cout << output[i] << " " ;
                            }
                            out << endl;
                            //cout<<endl;
                            //out << endl;
                            for(int i=0 ;i<4;i++)
                                output[i]=2;
                        }

                // enclose 2 elements

                        else
                        {
                            if(a==1)
                            {
                                output[0]=0;
                                output[1]=0;
                            }
                            if(a==2)
                            {
                                output[0]=0;
                                output[1]=1;
                            }
                            if(a==3)
                            {
                                output[0]=1;
                                output[1]=1;
                            }
                            if(a==4)
                            {
                                output[0]=1;
                                output[1]=0;
                            }
                            if(b==1)
                            {
                                output[2]=0;
                                output[3]=0;
                            }
                            if(b==2)
                            {
                                output[2]=0;
                                output[3]=1;
                            }
                            if(b==3)
                            {
                                output[2]=1;
                                output[3]=1;
                            }
                            if(b==4)
                            {
                                output[2]=1;
                                output[3]=0;
                            }

                            for(int i=0 ;i<4;i++)
                            {
                                out << output[i] << " " ;
                                //cout << output[i] << " " ;
                            }
                            out << endl;
                            //cout<<endl;
                            //out << endl;
                            for(int i=0 ;i<4;i++)
                                output[i]=2;
                        }

                // no elements can be enclosed

                    }
                }
            }
            judge=0;
            c=0;
        } // end if 4 variables
        Case--;
    }

}
