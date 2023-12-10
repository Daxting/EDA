#include "basic.h"
#include "visualization.h"
#include "foundation.h"
#include "rating.h"
#include "silkscreen_process.h"
#include "extremum.h"

int main() {
    timer timer1;
    bool insurance = true;
    //string file_location = "C:\\Users\\timta\\Desktop\\Laboratory\\PublicCase\\PublicCase_C.txt";
    string file_location = "TestCase19.txt";
    string output_file_name = "Result.txt";
    ifstream infile(file_location, std::ios::in);
    while (!infile)
        cout << "The file is not found. " << endl;
    double assemblygap, coppergap, silkscreenlen;
    int arc_number, line_number;
    io::get_data(infile, assemblygap, coppergap, silkscreenlen);
    // assembly
    component assembly("assembly");
    io::get_assembly_location(infile, assembly, arc_number, line_number);
    assembly.set_extreme_value();
    //coppers
    vector<component> coppers;
    io::get_copper_location(infile, coppers);
    rt::storeProblem(assembly, coppers, assemblygap, coppergap, silkscreenlen, line_number, arc_number); //save data to rating
    //deal with some odd feature in assembly
    assembly.eliminate_duplicate_arc();
    assembly.eliminateCcwSmallArc(assemblygap);
    assembly.eleminate_collinear_seg();
    //coppers buffer construction
    vector<component> coppbuf(0);
    vector<component> coppbuf_sharp(0);
    double copper_xmax = 0;
    double coppergapchange = coppergap;
    for (int i = 0; i < coppers.size(); i++)
    {

        component tempcoppbuf;
        coppers.at(i).eliminate_duplicate_arc();
        coppers.at(i).eliminateCcwSmallArc(coppergap);
        coppers.at(i).eleminate_collinear_seg();
        coppers.at(i).set_extreme_value();
        gm::create_material(coppers.at(i), tempcoppbuf, coppergap);
        while (!rt::testbuffer(tempcoppbuf, coppers.at(i), false)) {//test if the assbuf is legal
            coppergapchange += 0.00001;
            coppers.at(i).eliminateCcwSmallArc(coppergapchange);
            tempcoppbuf.material.clear();
            gm::create_material(coppers.at(i), tempcoppbuf, coppergapchange);
        }
        coppbuf_sharp.push_back(tempcoppbuf);
        gm::sharp_corner(coppers.at(i), tempcoppbuf);
        coppbuf.push_back(tempcoppbuf);
        coppbuf.at(i).set_extreme_value();
        //pc::draw(coppbuf.at(i), pc::lightblue);
        if (i == 0)
            copper_xmax = coppbuf.at(0).xmax;
        else if (copper_xmax < coppbuf.at(i).xmax)
            copper_xmax = coppbuf.at(i).xmax;
    }
    ex::storedata(assembly, coppbuf_sharp);
    //declare 
    component assbuf("assbuf");
    extremum_silkscreens extremums(assembly);//store assExtremum
    vector<component> silkscreen(0);
    component receiver("receiver"), tempsilk("tempsilk");
    double change = 0;
    bool first_run = true;

    do {
        //clear the memory
        assbuf.clear();
        receiver.clear();
        tempsilk.clear();
        for (int i = 0; i < silkscreen.size(); i++) {
            silkscreen.clear();
        }
        extremums.clear();
        //create assbuf
        gm::create_material(assembly, assbuf, assemblygap + change);
        while (first_run && !rt::testbuffer(assbuf, assembly, true)) {//test if the assbuf is legal
            assembly.eliminateCcwSmallArc(assemblygap + change);
            assbuf.material.clear();
            gm::create_material(assembly, assbuf, assemblygap + change);
            change += 0.00001;//increase the buffer gap subtly
        }
        first_run = false;
        sp::silkscreen_deletion(assbuf, coppbuf, receiver, copper_xmax);//receiver=assbuf-coppbuf
        if (insurance && receiver.material.size() == 0) {
            //this function is used to solve the situation of no silkscreen left after deletion
            change += 0.03;
            continue;
        }
        bool no_cut = false;
        sp::make_tempsilk(receiver, tempsilk, silkscreenlen, no_cut);//tempsilk is the silkscreen in receiver format without too short condition
        if (insurance && no_cut) {
            //this means the coppbuf has no intersection with assbuf
            sp::connect_to_copper(receiver, coppbuf, extremums);
        }
        if (arc_number != 0) {
            ex::extremum_silkscreen(coppbuf, tempsilk, assbuf, extremums, coppergapchange, coppers, 0);//deal with cases that lack of any extremums
            ex::extremum_silkscreen(coppbuf_sharp, tempsilk, assbuf, extremums, coppergapchange, coppers, 1);//deal with cases that lack of any extremums
            sp::delete_overlap_silkscreen(extremums.key_silk);
        }
        else if (arc_number == 0)
            ex::extremum_silkscreen(coppbuf_sharp, tempsilk, assbuf, extremums, coppergapchange, coppers, 1);//deal with cases that lack of any extremums
        sp::silkscreen_connection(receiver, silkscreen);//because of discontinuity, change receiver to silkscreen
        extremums.classify_extremum(silkscreen);//take all the silkscreen with assembly extremums values
        ex::area_optimization(extremums); //deal with the extreme containers to reduce the silk area
        change += 0.00001;//increase the buffer gap subtly
    } while (!rt::testkeysilk(extremums.key_silk, assbuf));//check if key_silk is far enough with assembly
    rt::testSilkCopperinterface(extremums.key_silk, true); //due to the rounding problem, the trimer will trime the SilkCopper interface subtly
    extremums.minlength_checker(silkscreenlen); //chech if the length are not long enough
    extremums.classify_key_silk();//sort the key_silk into containers
    cout << "After all the silkscreen get adjusted, ";
    timer1.getExecutionTime(true);
    cout << "-------Selecting Answers-------" << endl;
    bool pass = false, pass_grade_limit = false;;
    double grade = 0, grade_limit = 1;
    int count = 0;
    for (int i = 0; i < extremums.xmax_container.size(); i++) {
        for (int j = 0; j < extremums.xmin_container.size(); j++) {
            for (int k = 0; k < extremums.ymax_container.size(); k++) {
                for (int w = 0; w < extremums.ymin_container.size(); w++) {
                    extremums.pick(i, j, k, w);
                    count++;
                    sp::delete_overlap_silkscreen(extremums.ans_silk); //delete the overlapping silkscreens or unnecessary silkscreens
                    sp::numbers_optimization(extremums.ans_silk, line_number, arc_number); //only increase numbers by cutting and creating
                    if (!pass) {
                        io::silkscreen_output(extremums.ans_silk, output_file_name);
                        grade = rt::rating(output_file_name, true);
                        if (!db::equal(grade, 0)) {
                            pass = true;
                            cout << "First pass grade: " << grade << endl;
                            if (db::greater_or_equal(grade, grade_limit)) {
                                cout << "over the grade limit " << grade_limit << endl;
                                pass_grade_limit = true;
                                break;
                            }
                        }
                    }
                    else {
                        grade = rt::rating(extremums.ans_silk, false);
                        cout << count << " grade: " << grade << endl;
                        if (db::greater_or_equal(grade, grade_limit)) {
                            cout << "over the grade limit " << grade_limit << endl;
                            pass_grade_limit = true;
                            break;
                        }
                    }
                }
                if (pass_grade_limit)
                    break;
            }
            if (pass_grade_limit)
                break;
        }
        if (pass_grade_limit)
            break;
    }
    cout << "After " << count << " kinds of answers, the answer is picked, Grade: " << grade << ", ";
    timer1.getExecutionTime(true);
    cout << "------top grade rating info start------" << endl;
    rt::rating(output_file_name, true);
    cout << "------top grade rating info end------" << endl;
    assbuf.set_extreme_value();
    double Extremum[4] = { assbuf.xmax , assbuf.xmin, assbuf.ymax, assbuf.ymin };//[0]=xmax,[1]=xmin,[2]=ymax,[3]=ymin
    pc::visualization(file_location, output_file_name, true, Extremum);
    cout << "completed!" << endl;
    return 0;
}