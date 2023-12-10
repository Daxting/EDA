namespace sp {
    int arc_type(point p, Arc arc);
    int seg_type(point p, segment seg);
    bool in_component(point p, component comp, double x_max);
    void segmentation(vector<element>& silks, component tempcoppbuf);
    void silkscreen_deletion(component assbuf, vector<component> coppbuf, component& receiver, double x_max);
    void silkscreen_connection(component comp, vector<component>& silkscreen);
    void numbers_optimization(vector<component>& extremum_silkscreen, int linedifference, int arcdifference);
    void delete_overlap_silkscreen(vector<component>& extremum_silkscreen);
    bool identical_silkscreen(component a, component b);
    void make_tempsilk(component receiver, component& tempsilk, double linit, bool& no_cut);
    void connect_to_copper(component tempsilk, vector<component> copbuf, extremum_silkscreens& extremums);
    void subtle_extend(element& ele, bool Ishead, double ratio);
    void coppor_union(vector<component>& ori, vector<component>& uni, double x_max);

    void coppor_union(vector<component>& ori, vector<component>& uni, double x_max) {
        cout << "------start of coppor_union------" << endl;
        int a, b, c, d;
        bool bol = 0;
        for (int i = 0; i < ori.size(); i++) {
            for (int j = 0; j < ori[i].material.size(); j++) {
                for (int k = i + 1; k < ori.size(); k++) {
                    for (int p = 0; p < ori[k].material.size(); p++) {
                        if (gm::intersects(ori[i].material[j], ori[k].material[p])) {
                            a = i, b = j, c = k, d = p;
                            bol = 1;
                            break;
                        }
                    }
                    if (bol)
                        break;
                }
                if (bol)
                    break;
            }
            if (bol)
                break;
        }
        if (!bol) {
            cout << "------end of coppor_union------" << endl;
            return;
        }

        else {
            vector<component> temp;
            vector<component> temp2;
            component receiver("receiver"), receiver2("receiver2");
            temp.push_back(ori[c]);
            temp2.push_back(ori[a]);
            silkscreen_deletion(ori[a], temp, receiver, x_max);
            silkscreen_deletion(ori[c], temp2, receiver2, x_max);
            receiver.log();
            receiver2.log();
            int record = 0, open = receiver.material.size();
            component memory;
            for (int i = 0; i < receiver.material.size() - 1; i++) {
                if (receiver.material[i].getEndPoint() != receiver.material[i + 1].getStartPoint()) {
                    open = i + 1;
                }
            }
            for (int i = open; i < receiver.material.size(); i++) {
                memory.material.push_back(receiver.material[i]);
            }
            for (int i = receiver.material.size() - 1; i >= open; i--) {
                receiver.material.erase(receiver.material.begin() + i);
            }
            for (int i = 0; i < receiver2.material.size(); i++) {
                if (receiver2.material[i].getStartPoint() == receiver.material[receiver.material.size() - 1].getEndPoint()) {
                    record = i;
                    break;
                }
            }
            for (int i = record; i < receiver2.material.size(); i++) {
                receiver.material.push_back(receiver2.material[i]);
            }
            for (int i = 0; i < record; i++) {
                receiver.material.push_back(receiver2.material[i]);
            }
            for (int i = 0; i < memory.material.size(); i++) {
                receiver.material.push_back(memory.material[i]);
            }
            receiver.log();
            //system("pause");
            vector<component> one;
            silkscreen_connection(receiver, one);
            uni.push_back(one[0]);
            for (int i = 0; i < ori.size(); i++) {
                if (i != a && i != c) {
                    uni.push_back(ori[i]);
                }
            }
            ori = uni;
            uni.clear();
            coppor_union(ori, uni, x_max);
        }
    }

    void connect_to_copper(component tempsilk, vector<component> copbuf, extremum_silkscreens& extremums) {
        //find the closest copbuf point out of tempsilk
        //change the sequence of tempsilk and make a seg to connect to the element
        int assele = -1;
        bool valid = 1;
        int a = -1, b = -1;
        double length = 10000;
        vector<point> tp;
        tempsilk.set_extreme_value();
        for (int i = 0; i < copbuf.size(); i++) {
            for (int j = 0; j < copbuf[i].material.size(); j++) {
                for (int k = 0; k < tempsilk.material.size(); k++) {
                    if (in_component(copbuf[i].material[j].getStartPoint(), tempsilk, tempsilk.xmax + 1))
                        continue;
                    valid = 1;
                    element connection(copbuf[i].material[j].getStartPoint(), tempsilk.material[k].getStartPoint());
                    for (int p = 0; p < tempsilk.material.size(); p++) {
                        tp.clear();
                        gm::element_intersection(connection, tempsilk.material[p], tp);
                        if (tp.size() == 0) {
                            continue;
                        }
                        else if (tp.size() > 1) {
                            valid = 0;
                            break;
                        }
                        else if (tp.size() == 1 && tp[0] != tempsilk.material[k].getStartPoint()) {
                            valid = 0;
                            break;
                        }
                    }
                    if (!valid) continue;
                    for (int p = 0; p < copbuf.size(); p++) {
                        for (int q = 0; q < copbuf[p].material.size(); q++) {
                            tp.clear();
                            gm::element_intersection(connection, copbuf[p].material[q], tp);
                            if (tp.size() == 0) {
                                continue;
                            }
                            else if (tp.size() > 1) {
                                valid = 0;
                                break;
                            }
                            else if (tp.size() == 1 && tp[0] != copbuf[i].material[j].getStartPoint()) {
                                valid = 0;
                                break;
                            }
                        }
                    }
                    if (valid && connection.length() < length) {
                        length = connection.length();
                        assele = k;
                        a = i;
                        b = j;
                    }
                }
            }
        }
        if (a == -1 || b == -1 || assele == -1) {
            return;
        }
        component temp("temp");
        temp.material.push_back(element(copbuf[a].material[b].getStartPoint(), tempsilk.material[assele].getStartPoint()));
        for (int i = assele; i < tempsilk.material.size(); i++) {
            temp.material.push_back(tempsilk.material[i]);
        }
        for (int i = 0; i < assele; i++) {
            temp.material.push_back(tempsilk.material[i]);
        }
        extremums.key_silk.push_back(temp);
    }

    void subtle_extend(element& ele, bool Ishead, double ratio) {
        if (!ele.is_arc) {
            if (Ishead) {
                ele.seg.first = point(ele.seg.first.x + ((ratio * (-ele.seg.second.x + ele.seg.first.x)) / ele.seg.length()),
                    ele.seg.first.y + ((ratio * (-ele.seg.second.y + ele.seg.first.y)) / ele.seg.length()));
            }
            else {
                ele.seg.second = point(ele.seg.second.x + ((ratio * (-ele.seg.first.x + ele.seg.second.x)) / ele.seg.length()),
                    ele.seg.second.y + ((ratio * (-ele.seg.first.y + ele.seg.second.y)) / ele.seg.length()));
            }
            return;
        }
        else {
            double degree = ratio / 2 / M_PI / ele.arc.radius() * 360;
            if (Ishead) {
                double angle = ele.arc.start_angle();
                if (ele.arc.direction == 1)
                    angle -= degree;
                else
                    angle += degree;
                if (angle > 360)
                    angle -= 360;
                else if (angle < 0) {
                    angle += 360;
                }
                ele.arc.p = ele.arc.angle_to_point(angle);
            }
            else {
                double angle = ele.arc.end_angle();
                if (ele.arc.direction == 1)
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
    }

    void make_tempsilk(component receiver, component& tempsilk, double linit, bool& no_cut) {
        vector<component> temp;
        silkscreen_connection(receiver, temp);
        if (temp.size() == 1 && temp[0].material[0].getStartPoint() == temp[0].material[temp[0].material.size() - 1].getEndPoint())
            no_cut = true;
        for (int i = 0; i < temp.size(); i++) {
            if (temp[i].length() < linit)
                temp.erase(temp.begin() + i);
        }
        for (int i = 0; i < temp.size(); i++) {
            for (int j = 0; j < temp[i].material.size(); j++) {
                if (j == 0) {
                    subtle_extend(temp[i].material[j], true, 0.1);
                }
                if (j == temp[i].material.size() - 1) {
                    subtle_extend(temp[i].material[j], false, 0.1);
                }
                tempsilk.material.push_back(temp[i].material[j]);
            }
        }
    }

    void silkscreen_deletion(component assbuf, vector<component> coppbuf, component& receiver, double x_max) {
        // declaration of silkscreen
        receiver = assbuf;
        for (int i = 0; i < coppbuf.size(); i++) {
            // separate the silkscreen by the intersection with one copper
            segmentation(receiver.material, coppbuf[i]);
        }
        // keep the element if its midpoint is not in the copper's buffer
        for (int i = 0; i < receiver.material.size(); i++) {
            for (int j = 0; j < coppbuf.size(); j++) {
                if (in_component(receiver.material[i].midpoint(), coppbuf[j], x_max)) {
                    receiver.material.erase(receiver.material.begin() + i);
                    i--;
                    break;
                }
            }
        }
    }

    void segmentation(vector<element>& silks, component tempcoppbuf)
    {
        //the function will return the seperated silks in "parameter" silks
        vector<element> material;//material is served as a temp memory
        for (int j = 0; j < silks.size(); j++) {
            element silk = silks[j];
            bool intersects = false;
            for (int k = 0; k < tempcoppbuf.material.size(); k++) {
                if (gm::intersects(silk, tempcoppbuf.material[k]))
                    intersects = true;
            }
            if (intersects) {
                vector<point> silk_copp_intersection;
                for (int i = 0; i < tempcoppbuf.material.size(); i++) {
                    gm::element_intersection(silk, tempcoppbuf.material[i], silk_copp_intersection);
                    /*for (int j = 0; j < silk_copp_intersection.size();j++) {
                        cout << "[" << i << "] " << gm::wkt(silk_copp_intersection[j]) << endl;
                    }*/
                }

                point start;
                Arc temparc = silk.arc;
                element tempelement;
                if (silk.is_arc == 0) // line
                {
                    start = silk.seg.first;
                    tempelement.is_arc = 0;
                    while (silk_copp_intersection.size() > 0)
                    {
                        double mindistance = gm::distance(silk.seg.first, silk.seg.second);
                        int number = 0;
                        for (int i = 0; i < silk_copp_intersection.size(); i++)
                        {
                            if (mindistance > gm::distance(silk.seg.first, silk_copp_intersection.at(i)))
                            {
                                mindistance = gm::distance(silk.seg.first, silk_copp_intersection.at(i));
                                number = i;
                            }
                        }
                        tempelement.seg = segment{ start, silk_copp_intersection.at(number) };
                        material.push_back(tempelement);
                        start = silk_copp_intersection.at(number);
                        silk_copp_intersection.erase(silk_copp_intersection.begin() + number);
                    }
                    tempelement.seg = segment{ start, silk.seg.second };
                    material.push_back(tempelement);
                }
                else // arc
                {
                    tempelement.is_arc = 1;
                    temparc.direction = silk.arc.direction;
                    start = silk.arc.p;
                    double minangle = 0;
                    if (db::equal(silk.arc.p.y, silk.arc.q.y))
                        minangle = 360;
                    else if ((db::greater(silk.arc.find_angle(silk.arc.q), silk.arc.find_angle(silk.arc.p)) && silk.arc.direction == 0) || (db::less(silk.arc.find_angle(silk.arc.q), silk.arc.find_angle(silk.arc.p)) && silk.arc.direction == 1))
                        minangle = 360 - abs(silk.arc.find_angle(silk.arc.p) - silk.arc.find_angle(silk.arc.q));
                    else
                        minangle = abs(silk.arc.find_angle(silk.arc.p) - silk.arc.find_angle(silk.arc.q));
                    double maxangle = minangle;
                    while (silk_copp_intersection.size() > 0)
                    {
                        int number = 0;
                        for (int i = 0; i < silk_copp_intersection.size(); i++)
                        {
                            double tempangle = 0;
                            if ((db::greater(silk.arc.find_angle(silk_copp_intersection.at(i)), silk.arc.find_angle(silk.arc.p)) && silk.arc.direction == 0 || (db::less(silk.arc.find_angle(silk_copp_intersection.at(i)), silk.arc.find_angle(silk.arc.p)) && silk.arc.direction == 1)))
                                tempangle = 360 - abs(silk.arc.find_angle(silk.arc.p) - silk.arc.find_angle(silk_copp_intersection.at(i)));
                            else
                                tempangle = abs(silk.arc.find_angle(silk.arc.p) - silk.arc.find_angle(silk_copp_intersection.at(i)));
                            if (db::greater(minangle, tempangle))
                            {
                                minangle = tempangle;
                                number = i;
                            }
                        }
                        temparc.p = start;
                        temparc.q = silk_copp_intersection.at(number);
                        tempelement.arc = temparc;
                        material.push_back(tempelement);
                        start = silk_copp_intersection.at(number);
                        silk_copp_intersection.erase(silk_copp_intersection.begin() + number);
                        minangle = maxangle;
                    }
                    temparc.p = start;
                    temparc.q = silk.arc.q;
                    tempelement.arc = temparc;
                    material.push_back(tempelement);
                }
            }
            else
                material.push_back(silk);
        }

        //copy
        silks.clear();
        for (int i = 0; i < material.size(); i++) {
            silks.push_back(material[i]);
        }
    }

    int arc_type(point p, Arc arc) {
        // 2 stands for interior point
        // 1 stands for a relative maximum point
        // 0 stands for a relative minimum point
        if (p == arc.p) {
            double angle = arc.start_angle();
            if (arc.direction == 0) {
                if (db::greater(angle, 90) && db::less_or_equal(angle, 270)) {
                    return 0;
                }
                else
                    return 1;
            }
            else if (arc.direction == 1) {
                if (db::greater_or_equal(angle, 90) && db::less(angle, 270)) {
                    return 1;
                }
                else
                    return 0;
            }
        }
        else if (p == arc.q) {
            double angle = arc.end_angle();
            if (arc.direction == 0) {
                if (db::greater_or_equal(angle, 90) && db::less(angle, 270)) {
                    return 1;
                }
                else
                    return 0;
            }
            else if (arc.direction == 1) {
                if (db::greater(angle, 90) && db::less_or_equal(angle, 270)) {
                    return 0;
                }
                else
                    return 1;

            }
        }
        else {
            return 2;
        }
    }

    int seg_type(point p, segment seg) {
        // 2 stands for interior point
        // 1 stands for a relative maximum point
        // 0 stands for a relative minimum point
        if (db::equal(p.y, seg.ceilValue()))
            return 1;
        else if (db::equal(p.y, seg.groundValue()))
            return 0;
        else
            return 2;
    }

    bool in_component(point p, component comp, double x_max) {
        // create a ray from the point to the right and check the number of intersections
        // if it's odd, then the point is in the component
        // if it's even, the point is at the outside of the component
        comp.set_extreme_value();
        element ray(p, point(x_max + 1, p.y));
        vector<point> intersections;
        vector<point> temp;
        vector<int> type;
        bool collinearity = false;
        for (int i = 0; i < comp.material.size(); i++) {
            // the element is arc
            if (comp.material[i].is_arc) {
                // if the point is on the element
                if (comp.material[i].arc.on_arc(p)) {
                    return true;
                }
                gm::element_intersection(ray, comp.material[i], temp);
                for (int j = 0; j < temp.size(); j++) {
                    if (collinearity == true) {
                        intersections.push_back(intersections[intersections.size() - 1]);
                        collinearity = false;
                    }
                    else if (temp[j] != comp.material[i].arc.p && temp[j] != comp.material[i].arc.q &&
                        (db::equal(temp[j].y, comp.material[i].arc.ceilValue()) || db::equal(temp[j].y, comp.material[i].arc.groundValue()))) {
                        continue;
                    }
                    else {
                        intersections.push_back(temp[j]);
                    }
                    type.push_back(arc_type(temp[j], comp.material[i].arc));
                    if (db::equal(temp[j].y, comp.ymax) || db::equal(temp[j].y, comp.ymin)) {
                        return false;
                    }
                }
                temp.clear();
            }
            else {        // if the element is seg
                if (comp.material[i].seg.on_segment(p))
                    return true;
                else if (!comp.material[i].seg.on_segment(p) && comp.material[i].seg.collinear(p)) {
                    collinearity = true;
                    continue;
                }
                if (ray.seg.intersects(comp.material[i].seg)) {
                    if (collinearity == true) {
                        intersections.push_back(intersections[intersections.size() - 1]);
                        collinearity = false;
                    }
                    else {
                        intersections.push_back(gm::element_intersection(element(ray.seg.first, ray.seg.second),
                            element(comp.material[i].seg.first, comp.material[i].seg.second)));
                    }
                    type.push_back(seg_type(intersections[intersections.size() - 1], comp.material[i].seg));
                }
                if (intersections.size() != 0 && (db::equal(intersections[intersections.size() - 1].y, comp.ymax)
                    || db::equal(intersections[intersections.size() - 1].y, comp.ymin))) {
                    return false;
                }
            }
        }
        if (intersections.size() > 1) { //select the points
            if (collinearity == true) {//if the last element is a collinear line to point p
                intersections[0] = intersections[intersections.size() - 1];
            }
            for (int i = 0; i < intersections.size(); i++) {
                if (i == intersections.size() - 1) {//the last run in the loop
                    if (intersections[i] == intersections[0]) {//same point in a row means, the special case happened
                        if (type[i] == type[0] == 1 || type[i] == type[0] == 0) {//the point is on the corner
                            //maximum+maximum  or  minimum+minimum
                            intersections.erase(intersections.begin() + i);
                            intersections.erase(intersections.begin());
                        }
                        else if (type[i] + type[0] == 1) {// the point is on the different endpoint of the elements
                            //maximun+minimum  or  minimum+maximum
                            intersections.erase(intersections.begin() + i);
                        }
                    }
                }
                else if (intersections[i] == intersections[i + 1]) {//same point in a row means, the special case happened
                    if (type[i] == type[i + 1] == 1 || type[i] == type[i + 1] == 0) {//the point is on the corner
                        //maximum+maximum  or  minimum+minimum
                        intersections.erase(intersections.begin() + i + 1);
                        intersections.erase(intersections.begin() + i);
                        i--;
                    }
                    else if (type[i] + type[i + 1] == 1) { // the point is on the different endpoint of the elements
                        //maximun+minimum  or  minimum+maximum
                        intersections.erase(intersections.begin() + i);
                    }
                }
            }
        }
        if (intersections.size() % 2 == 0) {
            return false;
        }
        else {
            return true;
        }
    }

    void silkscreen_connection(component comp, vector<component>& silkscreen) {
        component temp;//store a continuous silkscreen
        temp.material.push_back(comp.material[0]);
        for (int i = 0; i < comp.material.size(); i++) {
            if (i == comp.material.size() - 1) {
                silkscreen.push_back(temp);
            }
            else if (comp.material[i].getEndPoint() != comp.material[i + 1].getStartPoint()) {
                silkscreen.push_back(temp);
                temp.material.clear();
                temp.material.push_back(comp.material[i + 1]);
            }
            else if (comp.material[i].getEndPoint() == comp.material[i + 1].getStartPoint()) {
                temp.material.push_back(comp.material[i + 1]);
            }
        }
        //check if head and tail are the same point
        if (silkscreen.size() != 1 && silkscreen[silkscreen.size() - 1].material[silkscreen[silkscreen.size() - 1].material.size() - 1].getEndPoint()
            == silkscreen[0].material[0].getStartPoint()) {
            for (int i = 0; i < silkscreen[0].material.size(); i++) {
                silkscreen[silkscreen.size() - 1].material.push_back(silkscreen[0].material[i]);
            }
            silkscreen.erase(silkscreen.begin());
        }
    }

    // change the quantity of silkscreen line and arc;  silkscreen_addition
    void numbers_optimization(vector<component>& extremum_silkscreen, int linedifference, int arcdifference) {
        //cout << "-------Number Optimization Start-------" << endl;
        //cout << "ass line: " << linedifference << " , ass arc: " << arcdifference << endl;
        int linenumber = linedifference, arcnumber = arcdifference;
        linedifference = -1 * linedifference;
        arcdifference = -1 * arcdifference;
        bool zeroline = true, zeroarc = true;
        for (int i = 0; i < extremum_silkscreen.size(); i++) { // calculate the quantity of seg and arc in extremum_silkscreen
            for (int j = 0; j < extremum_silkscreen.at(i).material.size(); j++) {
                if (extremum_silkscreen.at(i).material.at(j).is_arc == 0) {
                    ++linedifference;
                    zeroline = false;
                }
                else {
                    ++arcdifference;
                    zeroarc = false;
                }
            }
        }
        //cout << "silk line: " << linenumber + linedifference << " , silk arc: " << arcnumber + arcdifference << endl;
        if (zeroline && linedifference < 0) { // if there is no seg in silkscreen
            for (int i = 0; i < extremum_silkscreen.size(); i++) {
                for (int j = 0; j < extremum_silkscreen.at(i).material.size(); j++) {
                    if (j == 0) { // the first element in extremum_silkscreen
                        point vec1((extremum_silkscreen.at(i).material.at(j).arc.p.x - extremum_silkscreen.at(i).material.at(j).arc.c.x), (extremum_silkscreen.at(i).material.at(j).arc.p.y - extremum_silkscreen.at(i).material.at(j).arc.c.y));
                        point vec2(-1 * (vec1.y), (vec1.x));
                        if ((extremum_silkscreen.at(i).material.at(j).arc.direction == 0 && db::greater(gm::crossed_product(vec1, vec2), 0)) || (extremum_silkscreen.at(i).material.at(j).arc.direction == 1 && db::less(gm::crossed_product(vec1, vec2), 0))) {
                            vec2.x = vec1.y;
                            vec2.y = -1 * vec1.x;
                        }
                        element newseg;
                        newseg.is_arc = 0;
                        newseg.seg.first = extremum_silkscreen.at(i).material.at(j).arc.p;
                        Arc anglearc;
                        anglearc.direction = 1;
                        anglearc.c = newseg.seg.first;
                        anglearc.p.x = anglearc.c.x + 0.01;
                        anglearc.p.y = anglearc.c.y;
                        for (int k = 0; k < 19; k++) { // 180 degree
                            newseg.seg.second.x = newseg.seg.first.x + vec2.x * 0.01 / sqrt(vec2.x * vec2.x + vec2.y * vec2.y);
                            newseg.seg.second.y = newseg.seg.first.y + vec2.y * 0.01 / sqrt(vec2.x * vec2.x + vec2.y * vec2.y);
                            anglearc.q = newseg.seg.second;
                            if (rt::element_isLegal(newseg)) { // if seg is legal
                                element copyseg = newseg;
                                copyseg.seg.first = newseg.seg.second;
                                copyseg.seg.second = newseg.seg.first;
                                int times = 0;
                                for (int k = 0; k < (-1 * linedifference) / 2; k++) {
                                    extremum_silkscreen.at(i).material.insert(extremum_silkscreen.at(i).material.begin() + j, copyseg);
                                    extremum_silkscreen.at(i).material.insert(extremum_silkscreen.at(i).material.begin() + j, newseg);
                                    times += 2;
                                }
                                linedifference += times;
                                if ((-1 * linedifference) == 1) {
                                    extremum_silkscreen.at(i).material.insert(extremum_silkscreen.at(i).material.begin() + j, copyseg);
                                    ++linedifference;
                                }
                            }
                            else { // plus ten degrees
                                double plusangle = anglearc.end_angle();
                                plusangle = (plusangle + 10) * M_PI / 180.0;
                                vec2.x = anglearc.radius() * cos(plusangle);
                                vec2.y = anglearc.radius() * sin(plusangle);
                            }
                            if (linedifference == 0)
                                break;
                        }
                    }
                    if (j == (extremum_silkscreen.at(i).material.size() - 1) && linedifference != 0) { // the last element in silkscreen
                        point vec1((extremum_silkscreen.at(i).material.at(j).arc.q.x - extremum_silkscreen.at(i).material.at(j).arc.c.x), (extremum_silkscreen.at(i).material.at(j).arc.q.y - extremum_silkscreen.at(i).material.at(j).arc.c.y));
                        point vec2(-1 * (vec1.y), (vec1.x));
                        if ((extremum_silkscreen.at(i).material.at(j).arc.direction == 0 && db::less(gm::crossed_product(vec1, vec2), 0)) || (extremum_silkscreen.at(i).material.at(j).arc.direction == 1 && db::greater(gm::crossed_product(vec1, vec2), 0))) {
                            vec2.x = vec1.y;
                            vec2.y = -1 * vec1.x;
                        }
                        element newseg;
                        newseg.is_arc = 0;
                        newseg.seg.first = extremum_silkscreen.at(i).material.at(j).arc.q;
                        Arc anglearc;
                        anglearc.direction = 0;
                        anglearc.c = newseg.seg.first;
                        anglearc.p.x = anglearc.c.x + 0.01;
                        anglearc.p.y = anglearc.c.y;
                        for (int k = 0; k < 19; k++) { // 180 degree
                            newseg.seg.second.x = vec2.x * 0.01 / sqrt(vec2.x * vec2.x + vec2.y * vec2.y);
                            newseg.seg.second.y = vec2.y * 0.01 / sqrt(vec2.x * vec2.x + vec2.y * vec2.y);
                            anglearc.q = newseg.seg.second;
                            if (rt::element_isLegal(newseg)) { // if seg is legal
                                element copyseg = newseg;
                                copyseg.seg.first = newseg.seg.second;
                                copyseg.seg.second = newseg.seg.first;
                                int times = 0;
                                for (int k = 0; k < (-1 * linedifference) / 2; k++) {
                                    extremum_silkscreen.at(i).material.insert(extremum_silkscreen.at(i).material.begin() + j, copyseg);
                                    extremum_silkscreen.at(i).material.insert(extremum_silkscreen.at(i).material.begin() + j, newseg);
                                    times += 2;
                                }
                                linedifference += times;
                                if ((-1 * linedifference) == 1) {
                                    extremum_silkscreen.at(i).material.insert(extremum_silkscreen.at(i).material.begin() + j, copyseg);
                                    ++linedifference;
                                }
                            }
                            else { // reduce ten degrees
                                double plusangle = anglearc.end_angle();
                                plusangle = (plusangle - 10) * M_PI / 180.0;
                                vec2.x = anglearc.radius() * cos(plusangle);
                                vec2.y = anglearc.radius() * sin(plusangle);
                            }
                            if (linedifference == 0)
                                break;
                        }
                    }
                    if (linedifference == 0)
                        break;
                }
                if (linedifference == 0)
                    break;
            }
        }
        //cout << "After new line: " << endl << "line: " << linenumber + linedifference << " , arc: " << arcnumber + arcdifference << endl;
        if (zeroarc && arcdifference < 0) { // if there is no arc in silkscreen
            if (linedifference <= 0) { // if line is enough
                for (int i = 0; i < extremum_silkscreen.size(); i++) {
                    if (extremum_silkscreen.at(i).material.size() == 1)
                        continue;
                    for (int j = 1; j < extremum_silkscreen.at(i).material.size(); j++) {
                        point vec1(extremum_silkscreen.at(i).material.at(j - 1).seg.second.x - extremum_silkscreen.at(i).material.at(j - 1).seg.first.x, extremum_silkscreen.at(i).material.at(j - 1).seg.second.y - extremum_silkscreen.at(i).material.at(j - 1).seg.first.y);
                        point vec2(extremum_silkscreen.at(i).material.at(j).seg.second.x - extremum_silkscreen.at(i).material.at(j - 1).seg.first.x, extremum_silkscreen.at(i).material.at(j).seg.second.y - extremum_silkscreen.at(i).material.at(j - 1).seg.first.y);
                        if (db::less(gm::crossed_product(vec1, vec2), 0) && !db::equal(gm::crossed_product(vec1, vec2), 0)) {
                            element newarc;
                            point vec3(extremum_silkscreen.at(i).material.at(j).seg.second.x - extremum_silkscreen.at(i).material.at(j).seg.first.x, extremum_silkscreen.at(i).material.at(j).seg.second.y - extremum_silkscreen.at(i).material.at(j).seg.first.y);
                            point vec4(-1 * vec3.y, vec3.x);
                            point arcend((extremum_silkscreen.at(i).material.at(j).seg.first.x + vec3.x * 0.01 / sqrt(vec3.x * vec3.x + vec3.y * vec3.y)), (extremum_silkscreen.at(i).material.at(j).seg.first.y + +vec3.y * 0.01 / sqrt(vec3.x * vec3.x + vec3.y * vec3.y)));
                            segment deleteseg(extremum_silkscreen.at(i).material.at(j).seg.first, arcend);
                            point newpoint = deleteseg.midpoint();
                            newarc.is_arc = 1;
                            newarc.arc.p = extremum_silkscreen.at(i).material.at(j).seg.first;
                            newarc.arc.q = arcend;
                            newarc.arc.direction = 0;
                            if (db::less(gm::crossed_product(vec3, vec4), 0)) {
                                vec4.x = vec3.y;
                                vec4.y = -1 * vec3.x;
                            }
                            newarc.arc.c.x = newpoint.x + vec4.x * 10000 / sqrt(vec4.x * vec4.x + vec4.y * vec4.y);
                            newarc.arc.c.y = newpoint.y + vec4.y * 10000 / sqrt(vec4.x * vec4.x + vec4.y * vec4.y);
                            if (rt::element_isLegal(newarc)) { // if arc is legal
                                element copyarc = newarc;
                                copyarc.arc.p = newarc.arc.q;
                                copyarc.arc.q = newarc.arc.p;
                                copyarc.arc.direction = 1;
                                int times = 0;
                                for (int k = 0; k < (-1 * arcdifference) / 2; k++) {
                                    extremum_silkscreen.at(i).material.insert(extremum_silkscreen.at(i).material.begin() + j, copyarc);
                                    extremum_silkscreen.at(i).material.insert(extremum_silkscreen.at(i).material.begin() + j, newarc);
                                    times += 2;
                                }
                                arcdifference += times;
                                if ((-1 * arcdifference) == 1) {
                                    extremum_silkscreen.at(i).material.at(j + times).seg.first = arcend;
                                    extremum_silkscreen.at(i).material.insert(extremum_silkscreen.at(i).material.begin() + j + times, newarc);
                                    ++arcdifference;
                                }
                            }
                            if (arcdifference == 0)
                                break;
                        }
                    }
                    if (arcdifference == 0)
                        break;
                }
            }
            if (arcdifference < 0) { // if line is not enough
                bool create = false;
                for (int i = 0; i < extremum_silkscreen.size(); i++) {
                    for (int j = 0; j < extremum_silkscreen.at(i).material.size(); j++) {
                        element newarc;
                        newarc.is_arc = 1;
                        newarc.arc.p = extremum_silkscreen.at(i).material.at(j).seg.first;
                        newarc.arc.q = extremum_silkscreen.at(i).material.at(j).seg.second;
                        newarc.arc.direction = 0;
                        point newpoint = extremum_silkscreen.at(i).material.at(j).midpoint();
                        point vec1((extremum_silkscreen.at(i).material.at(j).seg.second.x - extremum_silkscreen.at(i).material.at(j).seg.first.x), (extremum_silkscreen.at(i).material.at(j).seg.second.y - extremum_silkscreen.at(i).material.at(j).seg.first.y));
                        point vec2(-1 * vec1.y, vec1.x);
                        if (!db::less(gm::crossed_product(vec1, vec2), 0)) {
                            vec2.x = vec1.y;
                            vec2.y = -1 * vec1.x;
                        }
                        newarc.arc.c.x = newpoint.x + (vec2.x) * 10000 / extremum_silkscreen.at(i).material.at(j).seg.first.distance(extremum_silkscreen.at(i).material.at(j).seg.second);
                        newarc.arc.c.y = newpoint.x + (vec2.y) * 10000 / extremum_silkscreen.at(i).material.at(j).seg.first.distance(extremum_silkscreen.at(i).material.at(j).seg.second);
                        if (rt::element_isLegal(newarc)) { // if arc is legal
                            extremum_silkscreen.at(i).material.at(j) = newarc;
                            --linedifference;
                            ++arcdifference;
                            create = true;
                        }
                        if (create)
                            break;
                    }
                    if (create)
                        break;
                }
            }
        }
        //cout << "After new arc: " << endl << "line: " << linenumber + linedifference << " , arc: " << arcnumber + arcdifference << endl;
        int denominatior = 1;
        while (linedifference != 0 || arcdifference != 0) { // extreme silkscreen segment and arc quantity modification
            double minlength = 0.100 / denominatior; // the minlength of a seg or an arc // can be modified
            if (linedifference < 0) { // if segments are not enough
                for (int i = 0; i < extremum_silkscreen.size(); i++) {
                    for (int j = 0; j < extremum_silkscreen.at(i).material.size(); j++) {
                        if (extremum_silkscreen.at(i).material.at(j).is_arc == 0 && extremum_silkscreen.at(i).material.at(j).length() >= minlength) { // if the element's length is enough
                            point tempmidpoint = extremum_silkscreen.at(i).material.at(j).midpoint();
                            element newelement; // create new seg
                            newelement = extremum_silkscreen.at(i).material.at(j);
                            newelement.seg.first = tempmidpoint;
                            extremum_silkscreen.at(i).material.at(j).seg.second = tempmidpoint;
                            extremum_silkscreen.at(i).material.insert(extremum_silkscreen.at(i).material.begin() + j + 1, newelement); // insert new seg behind the present seg
                            j--;
                            ++linedifference;
                            if (linedifference == 0) // seg are enough
                                break;
                        }
                    }
                    if (linedifference == 0) // seg are enough
                        break;
                }
            }
            if (arcdifference < 0) { // if arc are not enough
                for (int i = 0; i < extremum_silkscreen.size(); i++) {
                    for (int j = 0; j < extremum_silkscreen.at(i).material.size(); j++) {
                        if (extremum_silkscreen.at(i).material.at(j).is_arc == 1 && extremum_silkscreen.at(i).material.at(j).arc.length() > minlength) { // if the element's length is enough
                            point tempmidpoint = extremum_silkscreen.at(i).material.at(j).midpoint();
                            element newelement = extremum_silkscreen.at(i).material.at(j); // create new arc
                            newelement.arc.p = tempmidpoint;
                            extremum_silkscreen.at(i).material.at(j).arc.q = tempmidpoint;
                            extremum_silkscreen.at(i).material.insert(extremum_silkscreen.at(i).material.begin() + j + 1, newelement); // insert new arc behind the present arc
                            j--;
                            ++arcdifference;
                            if (arcdifference == 0) // arc are enough
                                break;
                        }
                    }
                    if (arcdifference == 0) // arc are enough
                        break;
                }
            }
            denominatior *= 2;
            if (db::less(minlength, 0.005)) // the minimum length of a seg or an arc // can be modified
                break;
        }
        //cout << "answer line: " << linenumber + linedifference << " , answer arc: " << arcnumber + arcdifference << endl;
        //cout << "-------Number Optimization End-------" << endl;
    }

    void delete_overlap_silkscreen(vector<component>& extremum_silkscreen) {
        for (int i = 0; i < extremum_silkscreen.size(); i++) {
            for (int j = i + 1; j < extremum_silkscreen.size(); j++) {
                if (identical_silkscreen(extremum_silkscreen[i], extremum_silkscreen[j])) {
                    extremum_silkscreen.erase(extremum_silkscreen.begin() + j);
                    j--;
                }
            }
        }
    }

    bool identical_silkscreen(component a, component b) {
        if (a.material.size() != b.material.size())
            return false;
        for (int i = 0; i < a.material.size(); i++) {
            if (a.material[i] != b.material[i])
                return false;
        }
        return true;
    }
}
#pragma once