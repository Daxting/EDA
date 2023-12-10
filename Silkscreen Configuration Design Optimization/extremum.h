namespace ex {
    void extremum_silkscreen(vector<component> coppbuf, component receiver, component assbuf, extremum_silkscreens& extremums,double coppergap, vector<component> coppers, bool issharp);
    bool extremum_missing_type(double ass_ext[4], double silk_ext[4], int type);
    element generate_extreme_line(double ass_ext[4], double copp_ext[4], int type);
    void intersects_extreme_line_copper(vector<component>& coppbuf, element extreme_line);
    void intersects_silk_copper(vector<component>& coppbuf, component receiver);
    void sort_ep(vector<component>& coppbuf);
    void connect_direction(vector<component>& coppbuf);
    bool generate_silkscreen(vector<component> coppbuf, component assbuf, vector<component>& result);
    void classify_solution(extremum_silkscreens& extremums, vector<component> result, int type);
    bool compare_distance(struct point_data a, struct point_data b);
    bool compare_angle(struct point_data a, struct point_data b);
    void area_optimization(extremum_silkscreens& extremums);
    void box_area(extremum_silkscreens& extremums, int type);


    inline double ground(double x) {
        x = floor(x * 10000.0);
        return x / 10000.0 - 0.0001;
    }
    inline double roof(double x) {
        x = ceil(x * 10000.0);
        return x / 10000.0 + 0.0001;
    }

    double ass_ext[4];
    double copp_ext[4];


    void storedata(component assembly, vector<component> coppbuf) {
        assembly.set_extreme_value();
        ass_ext[0] = roof(assembly.xmax), ass_ext[1] = ground(assembly.xmin), ass_ext[2] = roof(assembly.ymax), ass_ext[3] = ground(assembly.ymin);
        coppbuf[0].set_extreme_value();
        copp_ext[0] = coppbuf[0].xmax, copp_ext[1] = coppbuf[0].ymax, copp_ext[2] = coppbuf[0].xmin, copp_ext[3] = coppbuf[0].ymin;
        for (int i = 1; i < coppbuf.size(); i++) {
            coppbuf[i].set_extreme_value();
            if (coppbuf[i].xmax > copp_ext[0])
                copp_ext[0] = coppbuf[i].xmax;
            if (coppbuf[i].ymax > copp_ext[1])
                copp_ext[1] = coppbuf[i].ymax;
            if (coppbuf[i].xmin < copp_ext[2])
                copp_ext[2] = coppbuf[i].xmin;
            if (coppbuf[i].ymin < copp_ext[3])
                copp_ext[3] = coppbuf[i].ymin;
        }
        copp_ext[0] = roof(copp_ext[0]), copp_ext[1] = roof(copp_ext[1]), copp_ext[2] = ground(copp_ext[2]), copp_ext[3] = ground(copp_ext[3]);
    }

    void extremum_silkscreen(vector<component> coppbuf, component receiver, component assbuf, extremum_silkscreens& extremums
                                                                    ,double coppergap, vector<component> coppers,bool issharp) {
        vector<component> result;
        receiver.set_extreme_value();
        double silk_ext[4] = { receiver.xmax, receiver.xmin, receiver.ymax, receiver.ymin };
        // if there's still missing extremum
        //cout << "-------start of extreme problem-------" << endl;
        for (int i = 0; i < 4; i++) {
            // generate assembly extreme line
            if (!extremum_missing_type(ass_ext, silk_ext, i))
                continue;
            else
                cout << "missing type: " << i << endl;
            element extreme_line = generate_extreme_line(ass_ext, copp_ext, i);
            //pc::draw(extreme_line);
            intersects_extreme_line_copper(coppbuf, extreme_line);
            intersects_silk_copper(coppbuf, receiver);
            sort_ep(coppbuf);
            connect_direction(coppbuf);
            // the forth arguement decide whether drawing the solutions or not
            if (!generate_silkscreen(coppbuf, assbuf, result)) {
                //special case
                bool pass = true;
                vector<component>copper_union, temp;
                do {
                    pass = true;
                    copper_union = coppbuf;
                    sp::coppor_union(copper_union, temp, copp_ext[0]+1);

                    for (int j = 0; j < copper_union.size(); j++) {
                        for (int k = 0; k < copper_union[j].material.size(); k++) {
                            if (!rt::testcopp(copper_union[j].material[k])) {
                                coppbuf.clear();
                                pass = false;
                                coppergap += 0.00002;
                                for (int p = 0; p < coppers.size(); p++)
                                {
                                    //coppers.at(i).log();
                                    component tempcoppbuf;
                                    coppers.at(p).eliminateCcwSmallArc(coppergap);
                                    gm::create_material(coppers.at(p), tempcoppbuf, coppergap);
                                    if (!issharp) {
                                        gm::sharp_corner(coppers.at(p), tempcoppbuf);
                                    }
                                    coppbuf.push_back(tempcoppbuf);
                                }
                                break;
                            }
                        }
                        if (pass == false)
                            break;
                    }
                } while (pass==false);
                intersects_extreme_line_copper(copper_union, extreme_line);
                intersects_silk_copper(copper_union, receiver);
                sort_ep(copper_union);
                connect_direction(copper_union);
                generate_silkscreen(copper_union, assbuf, result);
            }
            for (int j = 0; j < coppbuf.size(); j++) {
                coppbuf[j].ep.clear();
            }
            for (int j = 0; j < result.size(); j++) {
                extremums.key_silk.push_back(result[j]);
            }
            result.clear();
        }
        //cout << "------- end of extreme problem-------" << endl;
    }

    void classify_solution(extremum_silkscreens& extremums, vector<component> result, int type) {
        // xmax
        if (type == 0) {
            for (int i = 0; i < result.size(); i++) {
                extremums.xmax_container.push_back(result[i]);
            }
        }
        // xmin
        else if (type == 1) {
            for (int i = 0; i < result.size(); i++) {
                extremums.xmin_container.push_back(result[i]);
            }
        }
        // ymax
        else if (type == 2) {
            for (int i = 0; i < result.size(); i++) {
                extremums.ymax_container.push_back(result[i]);
            }
        }
        // ymin
        else if (type == 3) {
            for (int i = 0; i < result.size(); i++) {
                extremums.ymin_container.push_back(result[i]);
            }
        }
    }

    bool extremum_missing_type(double ass_ext[4], double silk_ext[4], int type) {
        // xmax
        if (type == 0 && silk_ext[0] < ass_ext[0]) {
            return true;
        }
        // xmin
        else if (type == 1 && silk_ext[1] > ass_ext[1]) {
            return true;
        }
        // ymax
        else if (type == 2 && silk_ext[2] < ass_ext[2]) {
            return true;
        }
        // ymin
        else if (type == 3 && silk_ext[3] > ass_ext[3]) {
            return true;
        }
        // no extremum is missing
        else
            return false;
    }

    element generate_extreme_line(double ass_ext[4], double copp_ext[4], int type) {
        // generate a line on the assembly' extreme to find the intersection with the coppbuf or silk
        // getting coppbuf's extremum
        //cout << "generate_extreme_line: ";
        double ext[4] = { ass_ext[0], ass_ext[2], ass_ext[1], ass_ext[3] };
        // translating the extremum order
        if (type == 1)
            type = 2;
        else if (type == 2)
            type = 1;
        // type = 1 and 3, creating y_extreme
        if (type % 2) {
            double x1 = copp_ext[0], x2 = copp_ext[2], y = ext[type];
            //cout << element(point(x1, y), point(x2, y)).info() << endl;
            return element(point(x1, y), point(x2, y));
        }
        // type = 0 and 2, creating x_extreme
        else {
            double y1 = copp_ext[1], y2 = copp_ext[3], x = ext[type];
            //cout << element(point(x, y1), point(x, y2)).info() << endl;
            return element(point(x, y1), point(x, y2));
        }
    }

    void intersects_extreme_line_copper(vector<component>& coppbuf, element extreme_line) {
        //cout << "extreme line intersection: ";
        // traverse through coppers to testbuffert the intersects
        for (int i = 0; i < coppbuf.size(); i++) {
            for (int j = 0; j < coppbuf[i].material.size(); j++) {
                vector<point> intersections;
                gm::element_intersection(coppbuf[i].material[j], extreme_line, intersections);
                for (int k = 0; k < intersections.size(); k++) {
                   // cout << "(" << intersections[k].x << "," << intersections[k].y << "), ";
                    struct point_data temp;
                    temp.pt = intersections[k];
                    temp.number = j;
                    temp.type = 0; // extreme line intersection
                    coppbuf[i].ep.push_back(temp);
                }
            }
        }
       // cout << endl;
    }

    void intersects_silk_copper(vector<component>& coppbuf, component receiver) {
       // cout << "silkscreen intersection: ";
        // if the copper comp intersects with extreme line then find the silkscreen intersection
        // if the copper comp doesn't intersects then the isc comp size is 0
        for (int i = 0; i < coppbuf.size(); i++) {
            // check if the coppbuf component intersects extreme line
            if (coppbuf[i].ep.size()) {
                for (int j = 0; j < coppbuf[i].material.size(); j++) {
                    vector<point> intersections;
                    for (int k = 0; k < receiver.material.size(); k++) {
                        //cout << coppbuf[i].material[j].info() << endl;
                        //cout << receiver.material[k].info() << endl;
                        gm::element_intersection(coppbuf[i].material[j], receiver.material[k], intersections);
                    }
                    for (int k = 0; k < intersections.size(); k++) {
                        //cout << "(" << intersections[k].x << "," << intersections[k].y << "), ";
                        struct point_data temp;
                        temp.pt = intersections[k];
                        temp.number = j;
                        temp.type = 1; // silkscreen intersection
                        coppbuf[i].ep.push_back(temp);
                    }
                }
            }
        }
        //cout << endl;
    }

    bool compare_distance(struct point_data a, struct point_data b) {
        return (a.distance < b.distance);
    }

    bool compare_angle(struct point_data a, struct point_data b) {
        return (a.angle < b.angle);
    }

    void sort_ep(vector<component>& coppbuf) {
       // cout << "------start of Sort EP------" << endl;
        for (int i = 0; i < coppbuf.size(); i++) {
            if (coppbuf[i].ep.size() == 0)
                continue;
            int size = coppbuf[i].material.size(); // the number of elements in this component
            vector<struct point_data> temp;
            for (int j = 0; j < size; j++) {
                // sort the points in the copper element (i, j) and store in vector same_element
                vector<struct point_data> same_element;
                if (!coppbuf[i].material[j].is_arc) {
                    for (int k = 0; k < coppbuf[i].ep.size(); k++) {
                        if (coppbuf[i].ep[k].number == j) {
                            // set the distance
                            coppbuf[i].ep[k].distance = coppbuf[i].ep[k].pt.distance(coppbuf[i].material[j].getStartPoint());
                            same_element.push_back(coppbuf[i].ep[k]);
                        }
                    }
                    std::sort(same_element.begin(), same_element.end(), compare_distance);
                }
                else {
                    for (int k = 0; k < coppbuf[i].ep.size(); k++) {
                        if (coppbuf[i].ep[k].number == j) {
                            Arc arc = coppbuf[i].material[j].arc;
                            if (arc.direction)
                                coppbuf[i].ep[k].angle = arc.find_angle(coppbuf[i].ep[k].pt) - arc.start_angle();
                            else
                                coppbuf[i].ep[k].angle = arc.start_angle() - arc.find_angle(coppbuf[i].ep[k].pt);
                            if (coppbuf[i].ep[k].angle < 0)
                                coppbuf[i].ep[k].angle += 360;
                            // set the distance
                            same_element.push_back(coppbuf[i].ep[k]);
                        }
                    }
                    std::sort(same_element.begin(), same_element.end(), compare_angle);
                }
                for (int k = 0; k < same_element.size(); k++) {
                    temp.push_back(same_element[k]);
                }
            }
            coppbuf[i].ep = temp;
        }
        //for (int i = 0; i < coppbuf.size(); i++) {
        //    if (coppbuf[i].ep.size() == 0)
        //        continue;
        //    //cout << "copper " << i << ": " << endl;
        //    for (int j = 0; j < coppbuf[i].ep.size(); j++) {
        //        if (coppbuf[i].ep[j].type)
        //            cout << "silk: ";
        //        else
        //            cout << "extreme: ";
        //        cout << coppbuf[i].ep[j].number << " (" << coppbuf[i].ep[j].pt.x << "," << coppbuf[i].ep[j].pt.y << ")" << endl;
        //    }
        //    cout << endl;
        //}
        //cout << "------end of Sort EP------" << endl;
    }

    void connect_direction(vector<component>& coppbuf) {
        //cout << "------start of connect_direction------" << endl;
        for (int i = 0; i < coppbuf.size(); i++) {
            if (coppbuf[i].ep.size() == 0)
                continue;
            if (!coppbuf[i].ep[0].type) {
                // if the previous type is extreme and the next type is silk
                // then the direction is CW
                if (!coppbuf[i].ep[coppbuf[i].ep.size() - 1].type && coppbuf[i].ep[1].type)
                    coppbuf[i].ep[0].direction = 0;
                // if the previous type is silk and the next type is extreme
                // then the direction is CCW
                else if (coppbuf[i].ep[coppbuf[i].ep.size() - 1].type && !coppbuf[i].ep[1].type)
                    coppbuf[i].ep[0].direction = 1;
                // otherwise the point will not be taken to generate silkscreen
                else
                    coppbuf[i].ep[0].direction = 2;
            }
            for (int j = 1; j < coppbuf[i].ep.size() - 1; j++) {
                // if the point is an intersection with the extreme line
                if (!coppbuf[i].ep[j].type) {
                    // if the previous type is extreme and the next type is silk
                    // then the direction is CW
                    if (!coppbuf[i].ep[j - 1].type && coppbuf[i].ep[j + 1].type)
                        coppbuf[i].ep[j].direction = 0;
                    // if the previous type is silk and the next type is extreme
                    // then the direction is CCW
                    else if (coppbuf[i].ep[j - 1].type && !coppbuf[i].ep[j + 1].type)
                        coppbuf[i].ep[j].direction = 1;
                    // otherwise the point will not be taken to generate silkscreen
                    else
                        coppbuf[i].ep[j].direction = 2;

                }
            }
            if (!coppbuf[i].ep[coppbuf[i].ep.size() - 1].type) {
                // if the previous type is extreme and the next type is silk
                // then the direction is CW
                if (!coppbuf[i].ep[coppbuf[i].ep.size() - 2].type && coppbuf[i].ep[0].type)
                    coppbuf[i].ep[coppbuf[i].ep.size() - 1].direction = 0;
                // if the previous type is silk and the next type is extreme
                // then the direction is CCW
                else if (coppbuf[i].ep[coppbuf[i].ep.size() - 2].type && !coppbuf[i].ep[0].type)
                    coppbuf[i].ep[coppbuf[i].ep.size() - 1].direction = 1;
                // otherwise the point will not be taken to generate silkscreen
                else
                    coppbuf[i].ep[coppbuf[i].ep.size() - 1].direction = 2;
            }
        }
    /*    for (int i = 0; i < coppbuf.size(); i++) {
            if (coppbuf[i].ep.size() == 0)
                continue;
            cout << "copper " << i << ": " << endl;
            for (int j = 0; j < coppbuf[i].ep.size(); j++) {
                if (!coppbuf[i].ep[j].type)
                    cout << "extreme: ";
                cout << "copper[" << i << "].material[" << coppbuf[i].ep[j].number << "]" << endl;
                cout << "(" << coppbuf[i].ep[j].pt.x << "," << coppbuf[i].ep[j].pt.y << "),  direction: " << coppbuf[i].ep[j].direction << endl;
            }
        }
        cout << "------end of connect_direction------" << endl;*/
    }

    bool generate_silkscreen(vector<component> coppbuf, component assbuf, vector<component>& result) {
        //cout << "------start of generate silkscreen------" << endl;
        // find the point type is extreme
        // if the direction is CW, connect the silkscreen from the point to the next ep
        cout << "kk" << endl;
        for (int i = 0; i < coppbuf.size(); i++) {
            cout << "kk" << endl;
            if (coppbuf[i].ep.size() == 0)
                continue;
            for (int j = 0; j < coppbuf[i].ep.size(); j++) {
                if (coppbuf[i].ep[j].type)
                    continue;
                struct point_data start_point, end_point;
                if (j == coppbuf[i].ep.size() - 1) {
                    // CW direction
                    if (coppbuf[i].ep[j].direction == 0) {
                        start_point = coppbuf[i].ep[j];
                        end_point = coppbuf[i].ep[0];
                    }
                    // CCW direction
                    else if (coppbuf[i].ep[j].direction == 1) {
                        start_point = coppbuf[i].ep[j - 1];
                        end_point = coppbuf[i].ep[j];
                    }
                    else
                        break;
                }
                else if (j == 0) {
                    // CW direction
                    if (coppbuf[i].ep[j].direction == 0) {
                        start_point = coppbuf[i].ep[j];
                        end_point = coppbuf[i].ep[j + 1];
                    }
                    // CCW direction
                    else if (coppbuf[i].ep[j].direction == 1) {
                        start_point = coppbuf[i].ep[coppbuf[i].ep.size() - 1];
                        end_point = coppbuf[i].ep[j];
                    }
                    else
                        break;
                }
                else {
                    // CW direction
                    if (coppbuf[i].ep[j].direction == 0) {
                        start_point = coppbuf[i].ep[j];
                        end_point = coppbuf[i].ep[j + 1];
                    }
                    // CCW direction
                    else if (coppbuf[i].ep[j].direction == 1) {
                        start_point = coppbuf[i].ep[j - 1];
                        end_point = coppbuf[i].ep[j];
                    }
                    else
                        break;
                }
                if (start_point.number == end_point.number) {
                    component solution;
                    element new_element;
                    if (coppbuf[i].material[start_point.number].is_arc) {
                        new_element.is_arc = true;
                        new_element.arc = Arc(start_point.pt, end_point.pt, coppbuf[i].material[start_point.number].arc.c, coppbuf[i].material[start_point.number].arc.direction);
                        new_element.numbers = start_point.number;
                    }
                    else {
                        new_element.is_arc = false;
                        new_element.seg = segment(start_point.pt, end_point.pt);
                        new_element.numbers = start_point.number;
                    }
                    solution.material.push_back(new_element);
                    solution.log();
                    if (!rt::legal(solution)) {
                        cout << "illegal";
                        continue;
                    }
                    result.push_back(solution);
                    cout << "legal" << endl;
                }
                // the start point and end point is on different element
                else {
                    component solution;
                    element start, end;
                    // start element is consist of the start point and the element's end point
                    if (coppbuf[i].material[start_point.number].is_arc) {
                        start.is_arc = true;
                        start.arc = Arc(start_point.pt, coppbuf[i].material[start_point.number].getEndPoint(), coppbuf[i].material[start_point.number].arc.c, coppbuf[i].material[start_point.number].arc.direction);
                        start.numbers = start_point.number;
                    }
                    else {
                        start.is_arc = false;
                        start.seg = segment(start_point.pt, coppbuf[i].material[start_point.number].getEndPoint());
                        start.numbers = start_point.number;
                    }
                    solution.material.push_back(start);

                    // add the element in the middle
                    int index = start_point.number + 1;
                    if (index == coppbuf[i].material.size())
                        index = 0;
                    while (index != end_point.number) {
                        solution.material.push_back(coppbuf[i].material[index]);
                        index++;
                        if (index == coppbuf[i].material.size())
                            index = 0;
                    }

                    // the end element is consist of the element's start point and the silk point
                    if (coppbuf[i].material[end_point.number].is_arc) {
                        end.is_arc = true;
                        end.arc = Arc(coppbuf[i].material[end_point.number].getStartPoint(), end_point.pt, coppbuf[i].material[end_point.number].arc.c, coppbuf[i].material[end_point.number].arc.direction);
                        end.numbers = end_point.number;
                    }
                    else {
                        end.is_arc = false;
                        end.seg = segment(coppbuf[i].material[end_point.number].getStartPoint(), end_point.pt);
                        end.numbers = end_point.number;
                    }
                    solution.material.push_back(end);
                    solution.log();
                    if (!rt::legal(solution)) {
                        cout << "illegal";
                        continue;
                    }
                    result.push_back(solution);
                    cout << "legal" << endl;
                }
            }
        }
        if (result.size() == 0) {
            return false;
        }
        else
            return true;
       // cout << "------end of generate silkscreen------" << endl;
    }

    void area_optimization(extremum_silkscreens& extremums) {
        //cout << "------start of area_optimization------" << endl;
        for (int i = 0; i < 4; i++)
            box_area(extremums, i);
        //cout << "------end of area_optimization------" << endl;
    }

    void box_area(extremum_silkscreens& extremums, int type) {
        vector<component> box;
        if (type == 0) {
            box = extremums.xmax_box;
        }
        else if (type == 1) {
            box = extremums.xmin_box;
        }
        else if (type == 2) {
            box = extremums.ymax_box;
        }
        else if (type == 3) {
            box = extremums.ymin_box;
        }
        element extreme_line = generate_extreme_line(ass_ext, copp_ext, type);
        // find the intersection of extreme_line and the component in the box
        for (int i = 0; i < box.size(); i++) {
            for (int j = 0; j < box[i].material.size(); j++) {
                vector<point> intersections; // intersecton of element and extreme line
                gm::element_intersection(box[i].material[j], extreme_line, intersections);
                for (int k = 0; k < intersections.size(); k++) {
                    // sort the intersections by distance from the start point
                    struct point_data temp;
                    temp.pt = intersections[k];
                    temp.number = j;
                    temp.type = 0; // extreme line intersection
                    box[i].ep.push_back(temp);
                }
            }
        }
        sort_ep(box);
        for (int i = 0; i < box.size(); i++) {
            // devide box[i] by the intersections
            // if the component has any intersection
            if (box[i].ep.size()) {
                // the first component
                component first;
                int index = 0;
                while (index < box[i].ep[0].number) {
                    first.material.push_back(box[i].material[index]);
                    index++;
                }
                // create an element from the start point of the component to the first point
                element new_element;
                if (box[i].material[index].is_arc) {
                    new_element.is_arc = true;
                    new_element.arc = Arc(box[i].material[index].getStartPoint(), box[i].ep[0].pt, box[i].material[index].arc.c, box[i].material[index].arc.direction);
                }
                else {
                    new_element.is_arc = false;
                    new_element.seg = segment(box[i].material[index].getStartPoint(), box[i].ep[0].pt);
                }
                first.material.push_back(new_element);

                if (first.material.size()) {
                    first.set_extreme_value();
                    double value;
                    if (type == 0) value = first.xmax;
                    else if (type == 1) value = first.xmin;
                    else if (type == 2) value = first.ymax;
                    else if (type == 3) value = first.ymin;
                    if (db::equal(value, ass_ext[type])) {
                        extremums.key_silk.push_back(first);
                        //pc::draw(first);
                    }
                    else {
                        //pc::draw(first, pc::blue);
                    }
                }

                // the last component
                component last;
                index = box[i].ep[box[i].ep.size() - 1].number;
                if (box[i].material[index].is_arc) {
                    new_element.is_arc = true;
                    new_element.arc = Arc(box[i].ep[box[i].ep.size() - 1].pt, box[i].material[index].getEndPoint(), box[i].material[index].arc.c, box[i].material[index].arc.direction);
                }
                else {
                    new_element.is_arc = false;
                    new_element.seg = segment(box[i].ep[box[i].ep.size() - 1].pt, box[i].material[index].getEndPoint());
                }
                last.material.push_back(new_element);
                index++;
                while (index < box[i].material.size()) {
                    last.material.push_back(box[i].material[index]);
                    index++;
                }
                last.set_extreme_value();

                if (last.material.size()) {
                    last.set_extreme_value();
                    double value;
                    if (type == 0) value = last.xmax;
                    else if (type == 1) value = last.xmin;
                    else if (type == 2) value = last.ymax;
                    else if (type == 3) value = last.ymin;
                    if (db::equal(value, ass_ext[type])) {
                        extremums.key_silk.push_back(last);
                        //pc::draw(last);
                    }
                    else {
                        //pc::draw(last, pc::blue);
                    }
                }
            }
            // select the component with the same extremum and put into container
            else {
                extremums.key_silk.push_back(box[i]);
            }
        }
    }
}
