///////////////rating manual///////////////
//////////Note!
//The data of any elements will be rounded at the fourth decimal place
//the second parameter is ShowDetail, if "true" put, the function will print all its details, if nothing put, it will be "False" by default
//////////Attached function
// If the silkscreen in the same mark is not continuous, then the rating function will seperate the discontinuity to two silkscreens
// If the head or tail of any silkscreen is too close to any coppers, and the distance is just smaller than the limit gap by 0.0001, 
// then auto_trimed() will try to cut the silkscreen so that the silkscreen will be legal(length limit and extremum problem will be considered too)
// Also, after any change in the rating, the new silkscreen will be printed again!
//////////Storage!
// For saving time, store data from main function.
// The first data is from txt, but others are from vector<component> type
// Function usage:
// 1.storeProblem()
// it will be used to store basic info from case.txt
// 2.bool testbuffer(component buffer, component comp, bool IsAss);
// it will be used when assemblybuffer is first created, and when extremum problem is happened
// 3.extremums_trimer()
// it will be used to get the extremums data in containers trimed so that it can survive from rounding problem
//////////First call!
// call the function: rating(string answer, bool ShowDetail)
// with the answer file printed already, and put answerfile name as the first parameter
//////////Following call!
// call the function:  rating(vector<component> silkscreen, bool ShowDetail)
// the function will detect the legality of the silkscreen and compare the total score with the highest one ever uploaded
// if the new score succeeds the highest one, the function will print the new answer to the file "Result.txt"
// No matter the new score is higher or lower, return its grade in rating process
///////////////////////////////////////////
namespace rt {
    double rating(string answer, bool ShowDetail);
    double rating(vector<component> silkscreen, bool ShowDetail);
    bool element_isLegal(element ele);
    bool legal(component comp);
    bool auto_trimed(element ele, component& silk, bool Ishead, bool IsAss,bool IsSk);
    void subtle_trimed(element& ele, bool Ishead, double ratio);
    bool extremum_check(vector<component> silk);
    bool silkscreen_discontinuity(vector<component> silkscreen);
    void storeProblem(component ass, vector<component> cop, double assgap, double copgap, double silklength, int asslinenum, int assarcnum);
    bool testbuffer(component buffer, component comp, bool IsAss);
    bool testkeysilk(vector<component>& key_silk, component assbuf);
    void testSilkCopperinterface(vector<component>& silk, bool showdetail);
    void extremums_trimer(extremum_silkscreens& extremums, bool showdetail = false);
    inline int adjacent_number(int n, int size);
    void get_answer_data(ifstream& infile, vector<component>& silkscreen);
    int silkscreen_check(vector<component> silkscreen, double& silktotallen, int& silkline_num, int& silkarc_num, struct extremum& silk_extremum);
    int silkscreen_copper_avgdistance(double& S3, vector<component> silk);
    int silkscreen_assembly_avgdistance(double& S4, vector<component> silk);
    void silkscreen_similarity(double& S2, int silkline_num, int silkarc_num, double silktotallen);
    void silkscreen_area(double& S1, struct extremum& silk_extremum);
    void silkscreen_recovery(vector<component>& silkscreen);
    void roundingComponent(component& comp);

    class score {
    public:
        double total;
        double S1;
        double S2;
        double S3;
        double S4;
        score() {
            total = 0;
            S1 = 0;
            S2 = 0;
            S3 = 0;
            S4 = 0;
        }
        void upload(score current) {
            total = current.total;
            S1 = current.S1;
            S2 = current.S2;
            S3 = current.S3;
            S4 = current.S4;
        }
        void count_total() {
            total = S1 + S2 + S3 + S4;
        }
        void log() {
            // output the score data
            cout << "-------Score--------" << endl;
            cout << "silkscreen area (S1) : " << S1 << endl;
            cout << "silkscreen similarity (S2) : " << S2 << endl;
            cout << "silkscreen distance with coppers (S3) : " << S3 << endl;
            cout << "silkscreen distance with assembly (S4) : " << S4 << endl;
            total = S1 + S2 + S3 + S4;
            cout << "score : " << total << endl;
            cout << "------End Score-----" << endl;
        }
        bool compare (score newScore) {
            count_total();
            newScore.count_total();
            if (newScore.total > total)
                return true;
            else
                return false;
        }
    };

    struct extremum {
        double xmax;
        double xmin;
        double ymax;
        double ymin;
    };

    component assembly;
    vector<component> copper;
    double assemblygap = 0, coppergap = 0, silkLength = 0, assbufLength = 0, assbufArea = 0;
    int assline_num = 0, assarc_num = 0;
    score highest;
    bool showDetail = 0;
    bool using_silk_recovery = 0;
   

    double rating(string answer, bool ShowDetail = false)//call this function at first time rating
    {
        ifstream answerfile(answer,std::ios::in);
        using_silk_recovery = 0;
        showDetail = ShowDetail;
        vector<component> silkscreen;
        int evaluation = 0; // get 0 of score if one of the evlauation is incorrect
        int silkline_num = 0, silkarc_num = 0; // the quantity of the element(line, arc)
        double silktotallen = 0;
        extremum silk_extremum;
        score current;
        if (showDetail) {
            cout << "---------Rating Show Detail Mode---------" << endl;
        }
        get_answer_data(answerfile, silkscreen);
        evaluation = silkscreen_check(silkscreen, silktotallen, silkline_num, silkarc_num, silk_extremum);
        if (evaluation == 2)
        {
            if (showDetail) {
                cout << "One of the silkscreen is too short(#2)" << endl;
            }
            return 0;
        }
        if (assembly.xmax > silk_extremum.xmax || assembly.xmin<silk_extremum.xmin || assembly.ymax> silk_extremum.ymax || assembly.ymin < silk_extremum.ymin)
        {
            if (showDetail) {
                cout << "Assembly's Extremums is out of Silkscreen's Extremums.(#1)" << endl;
                cout << "type: "<<"assembly "<<"silkscreen" << endl;
                cout << "xmax: " <<assembly.xmax << "  " << silk_extremum.xmax << endl;
                cout << "xmin: " <<assembly.xmin << "  " << silk_extremum.xmin << endl;
                cout << "ymax: " <<assembly.ymax << "  " << silk_extremum.ymax << endl;
                cout << "ymin: " <<assembly.ymin << "  " << silk_extremum.ymin << endl;
            }
            return 0;
        }
        evaluation = silkscreen_copper_avgdistance(current.S3, silkscreen);
        if (evaluation == 4)
        {
            if (showDetail) {
                cout << " One of the silkscreen is too close to the coppers(#4)" << endl;
            }
            return 0;
        }
        evaluation = silkscreen_assembly_avgdistance(current.S4, silkscreen);
        if (evaluation == 3)
        {
            if (showDetail) {
                cout << " One of the silkscreen is too close to the assembly(#3)" << endl;
            }
            return 0;
        }
        silkscreen_similarity(current.S2, silkline_num, silkarc_num, silktotallen);
        silkscreen_area(current.S1, silk_extremum);
        if (using_silk_recovery) {
            io::silkscreen_output(silkscreen, "Result.txt");
            if (showDetail) {
                cout << "After silkscreen recovery, the answer is the first legal answer." << endl;
            }
        }
        if (showDetail) {
            current.log();
        }
        highest.upload(current);
        if (showDetail) {
            cout << "---------Rating Show Detail Over---------" << endl;
        }
        current.count_total();
        return current.total;
    }
    double rating(vector<component> silkscreen, bool ShowDetail = false)//call this function after the first time
    {
        for (int i = 0; i < silkscreen.size(); i++) {
            roundingComponent(silkscreen[i]);
        }
        int evaluation = 0; // get 0 of score if one of the evlauation is incorrect
        showDetail = ShowDetail;
        using_silk_recovery = 0;
        int silkline_num = 0, silkarc_num = 0; // the quantity of the element(line, arc)
        double silktotallen = 0;
        extremum silk_extremum;
        score current;
        if (showDetail) {
            cout << "---------Rating Show Detail Mode---------" << endl;
        }
        if (silkscreen_discontinuity(silkscreen)) {
            if (showDetail) {
                cout << "One of the silkscreen is not continuous(#5)" << endl;
            }
            silkscreen_recovery(silkscreen);
            using_silk_recovery = 1;
        }
        evaluation = silkscreen_check(silkscreen, silktotallen, silkline_num, silkarc_num, silk_extremum);
        if (evaluation == 2)
        {
            if (showDetail) {
                cout << " One of the silkscreen is too short(#2)" << endl;
            }
            return 0;
        }

        if (assembly.xmax> silk_extremum.xmax || assembly.xmin<silk_extremum.xmin || assembly.ymax> silk_extremum.ymax || assembly.ymin< silk_extremum.ymin)
        {
            if (showDetail) {
                cout << "Assembly's Extremums is out of Silkscreen's Extremums.(#1)" << endl;
                cout << "type: " << "assembly " << "silkscreen" << endl;
                cout << "xmax: " <<assembly.xmax << "  " << silk_extremum.xmax << endl;
                cout << "xmin: " <<assembly.xmin << "  " << silk_extremum.xmin << endl;
                cout << "ymax: " <<assembly.ymax << "  " << silk_extremum.ymax << endl;
                cout << "ymin: " <<assembly.ymin << "  " << silk_extremum.ymin << endl;
            }
            return 0;
        }
        evaluation = silkscreen_copper_avgdistance(current.S3, silkscreen);
        if (evaluation == 4)
        {
            if (showDetail) {
                cout << "  One of the silkscreen is too close to the coppers(#4)" << endl;
            }
            return 0;
        }
        evaluation = silkscreen_assembly_avgdistance(current.S4, silkscreen);
        if (evaluation == 3)
        {
            if (showDetail) {
                cout << " One of the silkscreen is too close to the assembly(#3)" << endl;
            }
            return 0;
        }
        silkscreen_similarity(current.S2, silkline_num, silkarc_num, silktotallen);
        silkscreen_area(current.S1, silk_extremum);
        if (showDetail) {
            current.log();
        }
        if (highest.compare( current)) {
            highest.upload(current);
            io::silkscreen_output(silkscreen, "Result.txt");
            if (using_silk_recovery && showDetail) {
                cout << "After silkscreen recovery, the answer is the legal answer with highest score." << endl;
            }
            else if (showDetail) {
                cout << "Yeah! It's the highest score ever!" << endl;
            }
        }
        if (showDetail) {
            cout << "---------Rating Show Detail Over---------" << endl;
        }
        current.count_total();
        return current.total;
    }
    bool legal(component comp) {
        for (int j = 0; j < comp.material.size(); j++) {
            for (int i = 0; i < assembly.material.size(); i++) {
                if (gm::element_distance(comp.material[j], assembly.material[i]) < assemblygap-0.0005) {
                    return 0;
                }
            }
        }
        for (int k = 0; k < comp.material.size(); k++) {
            for (int i = 0; i < copper.size(); i++) {
                for (int j = 0; j < copper[i].material.size(); j++) {
                    if (gm::element_distance(comp.material[k], copper[i].material[j]) < coppergap - 0.0005)
                        return 0;
                }
            }
        }
        return 1;
    }
    bool testcopp(element ele) {
        for (int i = 0; i < copper.size(); i++) {
            for (int j = 0; j < copper[i].material.size(); j++) {
                if (gm::element_distance(ele, copper[i].material[j]) < coppergap)
                    return 0;
            }
        }
        return 1;
    }
    bool element_isLegal(element ele) {
        //return 0 , means illegal
        //return 1 , means legal
        for (int i = 0; i < assembly.material.size(); i++) {
            if (gm::element_distance(ele, assembly.material[i])< assemblygap) {
                return 0;
            }
        }
        for (int i = 0; i < copper.size(); i++) {
            for (int j = 0; j < copper[i].material.size(); j++) {
                if (gm::element_distance(ele, copper[i].material[j])< coppergap)
                    return 0;
            }
        }
        return 1;
    }
    void roundingComponent(component& comp) {
        for (int i = 0; i < comp.material.size(); i++) {
            if (!comp.material[i].is_arc) {//seg
                comp.material[i].seg.first.x = db::rounding(comp.material[i].seg.first.x);
                comp.material[i].seg.first.y = db::rounding(comp.material[i].seg.first.y);
                comp.material[i].seg.second.x = db::rounding(comp.material[i].seg.second.x);
                comp.material[i].seg.second.y = db::rounding(comp.material[i].seg.second.y);
            }
            else {//arc
                comp.material[i].arc.c.x = db::rounding(comp.material[i].arc.c.x);
                comp.material[i].arc.c.y = db::rounding(comp.material[i].arc.c.y);
                comp.material[i].arc.p.x = db::rounding(comp.material[i].arc.p.x);
                comp.material[i].arc.p.y = db::rounding(comp.material[i].arc.p.y);
                comp.material[i].arc.q.x = db::rounding(comp.material[i].arc.q.x);
                comp.material[i].arc.q.y = db::rounding(comp.material[i].arc.q.y);
            }
        }
    }
    bool testkeysilk(vector<component> &key_silk, component assbuf) {
        if (key_silk.size() == 0)
            return false;
        for (int i = 0; i < key_silk.size(); i++) {
            roundingComponent(key_silk[i]);
        }
        for (int i = 0; i < key_silk.size(); i++) {
            for (int k = 0; k < key_silk[i].material.size(); k++) {
                for (int j = 0; j < assembly.material.size(); j++) {
                    if (gm::element_distance(key_silk[i].material[k], assembly.material[j])< assemblygap) {
                        return false;
                    }
                }
            }
        }
        assbufLength = assbuf.length();
        assbufArea = assbuf.area();
        return true;
    }
    bool testbuffer(component buffer, component comp, bool IsAss) {
        //buffer put the component which is create_buffered by comp
        //if IsAss puts true, means the function will use assemblygap as the gap, and store the info needed
        //      (it will only check the adjacent three elements in buffer
        //if Isass puts false, means the operation is about extremum_process
        //      (because sharp corner, it will check the adjacent five elements in buffer
        //Warning! The limit of the function is the numbers of the corresponded elements should not be chaotic
        roundingComponent(buffer);
        double gap=0;
        if (IsAss == true)
            gap = assemblygap;
        else if (IsAss == false)
            gap = coppergap;
        for (int i = 0; i < buffer.material.size(); i++) {//only examine the adjacent three element
            if (IsAss==false && gm::element_distance(buffer.material[i],
                comp.material[adjacent_number(buffer.material[i].numbers + 2, buffer.material.size())]) < gap ||
                gm::element_distance(buffer.material[i],
                    comp.material[adjacent_number(buffer.material[i].numbers - 2, buffer.material.size())])< gap) {
                return false;
            }
            if (gm::element_distance(buffer.material[i], comp.material[buffer.material[i].numbers])< gap ||
                gm::element_distance(buffer.material[i],
                    comp.material[adjacent_number(buffer.material[i].numbers + 1, buffer.material.size())])< gap ||
                gm::element_distance(buffer.material[i],
                    comp.material[adjacent_number(buffer.material[i].numbers - 1, buffer.material.size())])< gap) {
                return false;
            }
        }
        if (IsAss) {
            assbufLength = buffer.length();
            assbufArea = buffer.area();
        }
        return true;
    }
    void testSilkCopperinterface(vector<component>&silk, bool showdetail=false) {
        if (showdetail)
            cout << "------testSilkCopperinterface start------" << endl;
        double tempgap = 0;
        for (int i = 0; i < silk.size(); i++) // compare between the same silkscreen
        {
            for (int j = 0; j < silk.at(i).material.size(); j++)
            {
                for (int k = 0; k < copper.size(); k++)
                {
                    for (int l = 0; l < copper.at(k).material.size(); l++)
                    {
                        tempgap = gm::element_distance(silk.at(i).material.at(j), copper.at(k).material.at(l));
                        if (tempgap< coppergap && db::less_or_equal((coppergap - tempgap) , 0.0001) ) {
                            if (showdetail)
                                cout << "After SK trime, the silkscreen " << silk.at(i).material.at(j).info();
                            if (silk[i].material.size() == 1) {
                                if (auto_trimed(copper.at(k).material.at(l), silk.at(i), false, false,true)) {
                                    if (showdetail)
                                        cout << "is change to " << silk.at(i).material.at(j).info() << endl;
                                    continue;
                                }
                                if (auto_trimed(copper.at(k).material.at(l), silk.at(i), true, false,true)) {
                                    if (showdetail)
                                        cout << "is change to " << silk.at(i).material.at(j).info() << endl;
                                    continue;
                                }
                            }
                            else if (j == silk[i].material.size() - 1) {
                                if (auto_trimed(copper.at(k).material.at(l), silk.at(i), false, false,true)) {
                                    if (showdetail)
                                        cout << "is change to " << silk.at(i).material.at(j).info() << endl;
                                    continue;
                                }
                            }
                            else if (j == 0) {
                                if (auto_trimed(copper.at(k).material.at(l), silk.at(i), true, false,true)) {
                                    if (showdetail)
                                        cout << "is change to " << silk.at(i).material.at(j).info() << endl;
                                    continue;
                                }
                            }
                            if (showdetail) {
                                cout <<" is still to close to the copper " << copper.at(k).material.at(l).info() << endl;
                                cout << "testSilkCopperinterface can not solve" << endl;
                            }
                        }
                    }
                }
            }
        }
        if (showdetail)
            cout << "------testSilkCopperinterface end------" << endl;
        return;
    }
    inline int adjacent_number(int n, int size) {
        if (n < 0) {
            return adjacent_number(size + n,size);
        }
        else if (n >= size) {
            return adjacent_number(n-size, size);
        }
        else
            return n;
    }
    bool auto_trimed(element ele, component& silk, bool Ishead, bool IsAss, bool IsSK) {
        //ele is the element too close to the silkscreen
        //Ishead is true means silk.material[0], false means silk.material[size()-1]
        //detect if the element is too close to the head or tail of the silkscreen
        //if no, return false
        //if yes and the total length is still enough after auto_trimed, return yes
        //after the function, we will need to check if the extremum is changed
        //IsSK is true means the function will neglect the silk total length problem (too short)
        if(showDetail)
            cout << "Start Auto Trimed!" << endl;
        double gap = 0;
        if (IsAss)
            gap = assemblygap;
        else
            gap = coppergap;
        double length = 0.000051;
        double silk_original_length;
        point p;
        if (Ishead) {
            p = silk.material[0].getStartPoint();
            silk_original_length = silk.material[0].length();
            if (gm::distance(silk.material[0].getStartPoint(), ele)> gap) {
                if (showDetail)
                    cout << "Auto Trimed fail!(is not on the head)" << endl;
                return false;
            }
            while (gm::distance(silk.material[0].getStartPoint(), ele)< gap) {
                silk.material[0].setStartPoint(p);
                if (db::greater_or_equal(length, silk_original_length)) {
                    if (showDetail)
                        cout << "Auto Trimed fail!(impossible)" << endl;
                    return false;
                }
                subtle_trimed(silk.material[0], true, length);
                if (gm::element_distance(silk.material[0], ele)> gap) {
                    if (showDetail)
                        cout << "head is translate " << length << endl;
                    break;
                }
                length += 0.00005;
            }
        }
        else {
            p = silk.material[silk.material.size() - 1].getEndPoint();
            silk_original_length = silk.material[silk.material.size() - 1].length();
            if (gm::distance(silk.material[silk.material.size() - 1].getEndPoint(), ele)> gap) {
                if (showDetail)
                    cout << "Auto Trimed fail!(is not on the tail)" << endl;
                return false;
            }
            while (gm::distance(silk.material[silk.material.size() - 1].getEndPoint(), ele)< gap) {
                silk.material[silk.material.size() - 1].setEndPoint(p);
                if (db::greater_or_equal(length, silk_original_length)) {
                    if (showDetail)
                        cout << "Auto Trimed fail!(impossible)" << endl;
                    return false;
                }
                subtle_trimed(silk.material[silk.material.size() - 1], false, length);
                if (gm::element_distance(silk.material[silk.material.size() - 1], ele)> gap) {
                    if (showDetail)
                        cout << "tail is translate " << length << endl;
                    break;
                }
                length += 0.00005;
            }
        }
        if (!IsSK && db::less(silk.length(), silkLength)) {
            if (showDetail)
                cout << "Auto Trimed fail!(too short after trimed)" << endl;
            if (Ishead) {
                silk.material[0].setStartPoint(p);
            }
            else if (!Ishead) {
                silk.material[silk.material.size()-1].setEndPoint(p);
            }
            return false;
        }
        if (showDetail)
            cout << "Auto Trimed Succeed!" << endl;
        if(!IsSK)    
            using_silk_recovery = true;
        return true;
    }
    void subtle_trimed(element& ele, bool Ishead, double ratio) {
        if (!ele.is_arc) {
            if (ratio < ele.seg.length() * ele.seg.length()) {
                if (Ishead) {
                    ele.seg.first = point(ele.seg.first.x + ((ratio* (ele.seg.second.x- ele.seg.first.x))/ele.seg.length()),
                        ele.seg.first.y + ((ratio * (ele.seg.second.y - ele.seg.first.y)) / ele.seg.length()));
                }
                else {
                    ele.seg.second = point(ele.seg.second.x + ((ratio * (ele.seg.first.x - ele.seg.second.x)) / ele.seg.length()),
                        ele.seg.second.y + ((ratio * (ele.seg.first.y - ele.seg.second.y)) / ele.seg.length()));
                }
                return;
            }
            else
                return;
        }
        else {
            double degree= ratio / 2 / M_PI / ele.arc.radius() * 360;
            if (degree<ele.arc.arc_angle()) {
                if (Ishead) {
                    double angle = ele.arc.start_angle();
                    if (ele.arc.direction == 0)
                        angle-= degree;
                    else
                        angle+= degree;
                    if (angle > 360)
                        angle -= 360;
                    else if (angle < 0) {
                        angle += 360;
                    }
                    ele.arc.p = ele.arc.angle_to_point(angle);
                }
                else {
                    double angle = ele.arc.end_angle();
                    if (ele.arc.direction == 0)
                        angle += degree;
                    else
                        angle -= degree;
                    if (angle > 360)
                        angle -= 360;
                    else if (angle < 0) {
                        angle += 360;
                    }
                    ele.arc.q = ele.arc.angle_to_point(angle);
                }
                return;
            }
            else
                return;
        }
    }
    bool extremum_check(vector<component> silkscreen) {
        if (showDetail)
            cout << "Start Extremum check!" << endl;
        silkscreen[0].set_extreme_value();
        double xmax = silkscreen[0].xmax;
        double xmin = silkscreen[0].xmin;
        double ymax = silkscreen[0].ymax;
        double ymin = silkscreen[0].ymin;
        for (int i = 0; i < silkscreen.size(); i++) {
            silkscreen[i].set_extreme_value();
            if (silkscreen[i].xmax> xmax)
                xmax = silkscreen[i].xmax;
            if (xmin> silkscreen[i].xmin)
                xmin = silkscreen[i].xmin;
            if (silkscreen[i].ymax> ymax)
                ymax = silkscreen[i].ymax;
            if (ymin > silkscreen[i].ymin)
                ymin = silkscreen[i].ymin;
        }
        if (assembly.xmax> xmax ||assembly.xmin < xmin || assembly.ymax> ymax ||assembly.ymin < ymin)
        {
            if (showDetail)
                cout << "Extremum check fail!(the extremum is lost)" << endl;
            return false;
        }
        if (showDetail)
            cout << "Extremum check succeed!" << endl;
        return true;
    }
    bool silkscreen_discontinuity(vector<component> silkscreen) {
        for (int i = 0; i < silkscreen.size(); i++) {
            for (int j = 0; j < silkscreen[i].material.size() - 1; j++) {
                if (silkscreen[i].material[j].getEndPoint() != silkscreen[i].material[j + 1].getStartPoint()) {
                    if (showDetail) {
                        //pc::draw(newsilk, pc::blue);
                        cout << "One of the silkscreen is not continuous(#5)" << endl;
                        cout << "trouble happened at these silkscreen(#5):" << endl;
                        cout << silkscreen[i].material[j].info() << endl;
                        cout << silkscreen[i].material[j + 1].info()<<endl;
                        cout << "end of data" << endl;
                    }
                    return true;
                }
            }
        }
        return false;
    }
    void silkscreen_recovery(vector<component>& silkscreen) {
        if (showDetail) {
            cout << "start of silkscreen recovery!" << endl;
        }
        vector<component> substitute;
        component comp;
        if (showDetail) {
            cout << "before silkscreen recovery!" << endl;
            for (int i = 0; i < silkscreen.size(); i++) {
                silkscreen[i].log();
            }
        }
        for (int i = 0; i < silkscreen.size(); i++) {
            if (silkscreen[i].material.size() == 1) {
                comp.material.push_back(silkscreen[i].material[0]);
            }
            for (int j = 0; j < silkscreen[i].material.size() - 1; j++) {
                comp.material.push_back(silkscreen[i].material[j]);
                if (silkscreen[i].material[j].getEndPoint() != silkscreen[i].material[j + 1].getStartPoint()) {
                    substitute.push_back(comp);
                    comp.material.clear();
                    if (j == silkscreen[i].material.size() - 2) {
                        comp.material.push_back(silkscreen[i].material[j + 1]);
                    }
                }
            }
            substitute.push_back(comp);
            comp.material.clear();
        }
        silkscreen = substitute;
        if (showDetail) {
            cout << "after silkscreen recovery!" << endl;
            for (int i = 0; i < silkscreen.size(); i++) {
                silkscreen[i].log();
            }
        }
        if (showDetail) {
            cout << "end of silkscreen recovery!" << endl;
        }
    }
    void storeProblem(component ass, vector<component> cop, double assgap, double copgap, double silklength, int asslinenum, int assarcnum) {
        // storage some unchange data
        assembly=ass;
        component new_comp;
        for (int i = 0; i < cop.size(); i++) {
            copper.push_back(cop[i]);
        }
        assemblygap = assgap;
        coppergap = copgap;
        silkLength = silklength;
        assline_num = asslinenum;
        assarc_num = assarcnum;
        assembly.set_extreme_value();
    }
    void get_answer_data(ifstream& infile, vector<component>& silkscreen)
    {
        // read answer file
        bool troubleshooting = 0;
        string buffer;
        element newele;
        component newsilk;
        double x1, y1, x2, y2, xc, yc;
        infile >> buffer;
        while (infile.peek() != EOF)
        {
            infile >> buffer;
            while (buffer != "silkscreen" && infile.peek() != EOF)
            {
                if (buffer.substr(0, buffer.find(",")) == "line")
                {
                    buffer = buffer.substr(buffer.find(",") + 1);
                    x1 = db::rounding(stod(buffer.substr(0, buffer.find(","))));
                    buffer = buffer.substr(buffer.find(",") + 1);
                    y1 = db::rounding(stod(buffer.substr(0, buffer.find(","))));
                    buffer = buffer.substr(buffer.find(",") + 1);
                    x2 = db::rounding(stod(buffer.substr(0, buffer.find(","))));
                    buffer = buffer.substr(buffer.find(",") + 1);
                    y2 = db::rounding(stod(buffer.substr(0, buffer.find(","))));
                    buffer = buffer.substr(buffer.find(",") + 1);
                    newele = element(point(x1, y1), point(x2, y2));
                }
                else if (buffer.substr(0, buffer.find(",")) == "arc")
                {                    
                    buffer = buffer.substr(buffer.find(",") + 1);
                    x1 = db::rounding(stod(buffer.substr(0, buffer.find(","))));
                    buffer = buffer.substr(buffer.find(",") + 1);
                    y1 = db::rounding(stod(buffer.substr(0, buffer.find(","))));
                    buffer = buffer.substr(buffer.find(",") + 1);
                    x2 = db::rounding(stod(buffer.substr(0, buffer.find(","))));
                    buffer = buffer.substr(buffer.find(",") + 1);
                    y2 = db::rounding(stod(buffer.substr(0, buffer.find(","))));
                    buffer = buffer.substr(buffer.find(",") + 1);
                    xc = db::rounding(stod(buffer.substr(0, buffer.find(","))));
                    buffer = buffer.substr(buffer.find(",") + 1);
                    yc = db::rounding(stod(buffer.substr(0, buffer.find(","))));
                    buffer = buffer.substr(buffer.find(",") + 1);
                    if (buffer == "CW")
                        newele = element(point(x1, y1), point(x2, y2), point(xc, yc), 0);
                    else
                        newele = element(point(x1, y1), point(x2, y2), point(xc, yc), 1);
                }
                newsilk.material.push_back(newele);
                //troubleshooting 
                if (newsilk.material.size() != 1) {
                    if (newsilk.material[newsilk.material.size() - 1].getStartPoint() != newsilk.material[newsilk.material.size() - 2].getEndPoint()) {
                        troubleshooting = 1;
                        if (showDetail) {
                            //pc::draw(newsilk, pc::blue);
                            cout << "One of the silkscreen is not continuous(#5)" << endl;
                            cout << "trouble happened at this silkscreen(#5):" << endl;
                            newsilk.log();
                            cout << "end of data" << endl;
                        }
                    }
                }
                infile >> buffer;
            }   
            silkscreen.push_back(newsilk);
            newsilk.material.clear();
        }
        if (troubleshooting) {
            silkscreen_recovery(silkscreen);
            using_silk_recovery = 1;
        }
    }
    int silkscreen_check(vector<component> silkscreen, double& silktotallen, int& silkline_num, int& silkarc_num, struct extremum& silk_extremum) {
        // silkscreen extremeum, silkscreen length check, check start and end point
        silkscreen[0].set_extreme_value();
        double xmax = silkscreen[0].xmax;
        double xmin = silkscreen[0].xmin;
        double ymax = silkscreen[0].ymax;
        double ymin = silkscreen[0].ymin;
        for (int i = 0; i < silkscreen.size(); i++) {
            if (silkscreen[i].length()< silkLength) {
                if (showDetail) {
                    cout << "trouble happened at this silkscreen(#2):" << endl;
                    cout << "Limit length: " << silkLength << endl;
                    cout << "length: " << silkscreen[i].length() << endl;
                    silkscreen[i].log();
                    cout << "end of data" << endl;
                }
                return 2;//任一絲印標示中單一連續線段之長度未大於等於絲印標示最短長度限制(#2)
            }
            for (int j = 0; j < silkscreen[i].material.size(); j++) {
                if (silkscreen[i].material[j].is_arc == 1)
                    silkarc_num += 1;
                else
                    silkline_num += 1;
            }
            silktotallen += silkscreen[i].length();
            silkscreen[i].set_extreme_value();
            if (silkscreen[i].xmax> xmax)
                xmax = silkscreen[i].xmax;
            if (xmin> silkscreen[i].xmin)
                xmin = silkscreen[i].xmin;
            if (silkscreen[i].ymax> ymax)
                ymax = silkscreen[i].ymax;
            if (ymin> silkscreen[i].ymin)
                ymin = silkscreen[i].ymin;
        }
        silk_extremum.xmax = xmax;
        silk_extremum.xmin = xmin;
        silk_extremum.ymax = ymax;
        silk_extremum.ymin = ymin;
        return 0;
    }
    int silkscreen_copper_avgdistance(double& S3, vector<component> silk)
    {
        // calculate the distance between silkscreen and copper (S3)
        double avgcoppergap = 0, mingap = -1, tempgap = 0;
        if (showDetail) {
            cout << "----------" << endl;
            cout << "S3: " << endl;
        }
        for (int i = 0; i < silk.size(); i++) // compare between the same silkscreen
        {
            for (int j = 0; j < silk.at(i).material.size(); j++)
            {
                for (int k = 0; k < copper.size(); k++)
                {
                    for (int l = 0; l < copper.at(k).material.size(); l++)
                    {
                        tempgap = gm::element_distance(silk.at(i).material.at(j), copper.at(k).material.at(l));
                        if (mingap == -1 || mingap > tempgap) // if it is the minimum
                        {
                            mingap = tempgap;
                            if (mingap < coppergap){
                                if (showDetail) {
                                    cout << "Problem happened at these elements(#4): " << endl;
                                    cout << "silkscreen element: " << silk.at(i).material.at(j).info() << endl;
                                    cout << "copper element: " << copper.at(k).material.at(l).info() << endl;
                                    cout << "Too close: " << mingap << " < " << coppergap << endl;
                                    //pc::draw(silk.at(i).material.at(j), pc::blue);
                                    //pc::draw(copper.at(k).material.at(l), pc::blue);
                                }
                                if (db::less_or_equal(coppergap - mingap, 0.0001)) {
                                    if (silk[i].material.size() == 1) {
                                        if (auto_trimed(copper.at(k).material.at(l), silk.at(i), false, false, false)
                                            && extremum_check(silk)) {
                                            mingap = gm::element_distance(silk.at(i).material.at(j), copper.at(k).material.at(l));
                                            continue;
                                        }
                                        if (auto_trimed(copper.at(k).material.at(l), silk.at(i), true, false, false)
                                            && extremum_check(silk)) {
                                            mingap = gm::element_distance(silk.at(i).material.at(j), copper.at(k).material.at(l));
                                            continue;
                                        }
                                    }
                                    else if (j == silk[i].material.size() - 1) {
                                        if (auto_trimed(copper.at(k).material.at(l), silk.at(i), false,false, false)
                                            && extremum_check(silk)) {
                                            mingap = gm::element_distance(silk.at(i).material.at(j), copper.at(k).material.at(l));
                                            continue;
                                        }
                                    }
                                    else if (j == 0) {
                                        if (auto_trimed(copper.at(k).material.at(l), silk.at(i), true, false, false)
                                            && extremum_check(silk)) {
                                            mingap = gm::element_distance(silk.at(i).material.at(j), copper.at(k).material.at(l));
                                            continue;
                                        }
                                    }
                                }
                                if (showDetail) {
                                    //pc::draw(silk.at(i).material.at(j));
                                    //pc::draw(copper.at(k).material.at(l));
                                }
                                return 4;
                            }
                        }
                    }
                }
            }
            avgcoppergap += mingap;
            if (showDetail) {
                cout << "silkscreen[" << i << "]'s distance: " << mingap << endl;
            }
            mingap = -1;
        }
        avgcoppergap = avgcoppergap / silk.size();
        S3 = (1 - (avgcoppergap - coppergap) * 10 / coppergap )* 0.25;
        if (showDetail) {
            cout << "silkscreen number: " << silk.size() << endl;
            cout << "Average copper gap: " << avgcoppergap << endl;
            cout << "----------" << endl;
        }
        if (db::greater(S3, 0.25)) S3 = 0.25;
        else if (db::less(S3, 0))    S3 = 0;
        return 0;
    }
    int silkscreen_assembly_avgdistance(double& S4, vector<component> silk)
    {
        // calculate the distance between silkscreen and assembly (S4)
        double avgassemblygap = 0, mingap = -1, tempgap = 0;
        if (showDetail) {
            cout << "----------" << endl;
            cout << "S4: " << endl;
        }
        for (int i = 0; i < silk.size(); i++) // compare between the same silkscreen
        {
            for (int j = 0; j < silk.at(i).material.size(); j++)
            {
                for (int k = 0; k < assembly.material.size(); k++)
                {
                    tempgap = gm::element_distance(silk.at(i).material.at(j), assembly.material.at(k)); // calculate the distance between line and line
                    if (mingap == -1 || mingap > tempgap) // if tempgap does not have value or if tempgap is bigger templinegap
                    {
                        mingap = tempgap;
                        if (mingap < assemblygap) {
                            if (showDetail) {
                                cout << "Problem happened at these elements(#3): " << endl;
                                cout << "silkscreen element: " << silk.at(i).material.at(j).info() << endl;
                                cout << "assembly element: " << assembly.material.at(k).info() << endl;
                                cout << "Too close: " << mingap << " < " << assemblygap << endl;
                                //pc::draw(silk.at(i).material.at(j), pc::blue);
                                //pc::draw(assembly.material.at(k), pc::blue);
                            }
                            if (db::less_or_equal(assemblygap - mingap, 0.0001)) {
                                if (silk[i].material.size() == 1) {
                                    if (auto_trimed(assembly.material.at(k), silk.at(i), false, true, false) && extremum_check(silk)) {
                                        mingap = gm::element_distance(silk.at(i).material.at(j), assembly.material.at(k));
                                        continue;
                                    }
                                    if (auto_trimed(assembly.material.at(k), silk.at(i), true, true, false) && extremum_check(silk)) {
                                        mingap = gm::element_distance(silk.at(i).material.at(j), assembly.material.at(k));
                                        continue;
                                    }
                                }
                                else if (j == silk[i].material.size() - 1) {
                                    if (auto_trimed(assembly.material.at(k), silk.at(i), false,true, false) && extremum_check(silk)) {
                                        mingap = gm::element_distance(silk.at(i).material.at(j), assembly.material.at(k));
                                        continue;
                                    }
                                }
                                else if (j == 0) {
                                    if (auto_trimed(assembly.material.at(k), silk.at(i), true, true, false) && extremum_check(silk)) {
                                        mingap = gm::element_distance(silk.at(i).material.at(j), assembly.material.at(k));
                                        continue;
                                     }
                                }
                            }
                            if (showDetail) {
                                //pc::draw(silk.at(i).material.at(j));
                                //pc::draw(assembly.material.at(k));
                            }
                            return 3;
                        }
                    }
                }
            }
            avgassemblygap += mingap;
            if (showDetail) {
                cout << "silkscreen[" << i << "]'s distance: " << mingap << endl;
            }
            mingap = -1;
        }
        avgassemblygap /= silk.size();
        S4 = (1 - (avgassemblygap - assemblygap) * 10 / assemblygap) * 0.25;
        if (showDetail) {
            cout << "silkscreen number: " << silk.size() << endl;
            cout << "Average assembly gap: " << avgassemblygap << endl;
            cout << "----------" << endl;
        }
        if (db::greater(S4, 0.25)) S4 = 0.25;
        else if (db::less(S4, 0)) S4 = 0;
        return 0;
    }
    void silkscreen_similarity(double& S2, int silkline_num, int silkarc_num, double silktotallen)
    {
        // calculate the second score (S2)
        double temp = (2 - silktotallen / assbufLength) * 0.15;
        if (db::greater(temp, 0.15)) S2 += 0.15;
        else S2 += temp;
        temp = (1 - (abs(assline_num - silkline_num) + abs(assarc_num - silkarc_num)) / static_cast<double>(assline_num + assarc_num + copper.size())) * 0.1;
        if (db::greater(temp, 0.1)) S2 += 0.1;
        else S2 += temp;
        if (showDetail) {
            cout << "----------" << endl;
            cout << "S2:" << endl;
            cout << "Assembly buffer Length: " << assbufLength << endl;
            cout << "Silkscreen total Length: " << silktotallen << endl;
            cout << "Length Part: " << (2 - silktotallen / assbufLength) * 0.15 << endl;
            cout << "Assembly line number: " << assline_num << endl;
            cout << "Assembly arc number: " << assarc_num << endl;
            cout << "Silkscreen line number: " << silkline_num << endl;
            cout << "Silkscreen arc number: " << silkarc_num << endl;
            cout << "Similarity Part: " << temp << endl;
            cout << "total: " << S2 << endl;
            cout << "----------" << endl;
        }
        if (S2 < 0)
            S2 = 0;
    }
    void silkscreen_area(double& S1, struct extremum& silk_extremum)
    {
        // calculate the first score (S1)
        double silkarea = (silk_extremum.xmax - silk_extremum.xmin) * (silk_extremum.ymax - silk_extremum.ymin); 
        S1 = (2 - silkarea / assbufArea) * 0.25;
        if (db::greater(S1, 0.25))  S1 = 0.25;
        if (showDetail) {
            cout << "----------" << endl;
            cout << "Area Detail:" << endl;
            cout << "silksrceen xmax: " << silk_extremum.xmax << endl;
            cout << "silksrceen xmin: " << silk_extremum.xmin << endl;
            cout << "silksrceen ymax: " << silk_extremum.xmax << endl;
            cout << "silksrceen ymin: " << silk_extremum.ymin << endl;
            cout << " Area of assbuf: " << assbufArea << endl;
            cout << "----------" << endl;
        }
    }
}

#pragma once
