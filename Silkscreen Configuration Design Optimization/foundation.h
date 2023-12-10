namespace gm {
    double crossed_product(point v1, point v2);
    int orientation(point p, point q, point r); // 0. collinear 1. clockwise 2. counterclockwise
    double distance(segment seg1, segment seg2);
    double distance(point p, point q);
    double distance(point p, segment seg);
    double distance(point p, Arc arc);
    double distance(point p, element ele);
    double element_distance(element e1, element e2);
    void seg_translate(segment seg, double& a, double& b, double& c, double gap);//buffered line equation:ax+by+c=0
    point find_ArcLine_intersection(Arc arc, segment seg, bool IsArcLead, double gap);//find_ArcLine_intersection(arc, line info,sequence,gap)
    point find_ArcArc_intersection(element e1, element e2, double gap);
    void find_ArcLine_intersection(Arc arc, segment seg, point& p1, point& p2);
    void find_ArcArc_intersection(element e1, element e2, point& p1, point& p2);
    point shifting_point(point p, point q, double d);
    bool collinear(point a, point b, point c);
    point element_intersection(element e1, element e2);
    void create_material(component source, component& receiver, double gap);//create_material(data from the document,buffered data)
    void closest_points(point p, segment seg, segment& result);
    bool on_segment(point p, point q, point r);
    bool intersects(element e1, element e2);
    void element_intersection(element e1, element e2, vector<point>& reciever);
    void sharp_corner(component& cop, component& copbuf);

    void sharp_corner(component& cop, component& copbuf) {
        double number1 = 0, number2 = 1;
        bool finish[2] = { 0, 0 };
        element newelement;
        newelement.is_arc = 1;
        newelement.numbers = -1;
        if (copbuf.material.size() == 1)
            return;
        int present = 1, before = 0, last = 0, end = number2;
        while (present <= copbuf.material.size()) {
            if (copbuf.material.at(before).numbers == -1 || copbuf.material.at(present).numbers == -1) {
                ++present;
                ++before;
                continue;
            }
            if (copbuf.material.at(before).is_arc == 0 && copbuf.material.at(present).is_arc == 0) {
                point vec1(copbuf.material.at(before).seg.second.x - copbuf.material.at(before).seg.first.x, copbuf.material.at(before).seg.second.y - copbuf.material.at(before).seg.first.y), vec2(copbuf.material.at(present).seg.second.x - copbuf.material.at(before).seg.first.x, copbuf.material.at(present).seg.second.y - copbuf.material.at(before).seg.first.y);
                if (db::less(gm::crossed_product(vec1, vec2), 0) && !db::equal(gm::crossed_product(vec1, vec2), 0)) {
                    newelement.arc.c = cop.material.at(number2).seg.first;
                    double fraction = gm::distance(cop.material.at(number1).seg.second, copbuf.material.at(before).seg) / cop.material.at(number1).seg.length();
                    point vertical_vec1((cop.material.at(number1).seg.second.y - cop.material.at(number1).seg.first.y), -1 * (cop.material.at(number1).seg.second.x - cop.material.at(number1).seg.first.x));
                    if (db::less(gm::crossed_product(vec1, vertical_vec1), 0)) {
                        vertical_vec1.x = -1 * (cop.material.at(number1).seg.second.y - cop.material.at(number1).seg.first.y);
                        vertical_vec1.y = cop.material.at(number1).seg.second.x - cop.material.at(number1).seg.first.x;
                    }
                    newelement.arc.p.x = newelement.arc.c.x + vertical_vec1.x * fraction;
                    newelement.arc.p.y = newelement.arc.c.y + vertical_vec1.y * fraction;
                    copbuf.material.at(before).seg.second = newelement.arc.p;
                    fraction = gm::distance(cop.material.at(number2).seg.first, copbuf.material.at(present).seg) / cop.material.at(number2).seg.length();
                    point vec3((copbuf.material.at(present).seg.second.x - copbuf.material.at(present).seg.first.x), (copbuf.material.at(present).seg.second.y - copbuf.material.at(present).seg.first.y));
                    point vertical_vec2((cop.material.at(number2).seg.second.y - cop.material.at(number2).seg.first.y), -1 * (cop.material.at(number2).seg.second.x - cop.material.at(number2).seg.first.x));
                    if (db::less(gm::crossed_product(vec3, vertical_vec2), 0)) {
                        vertical_vec2.x = -1 * (cop.material.at(number2).seg.second.y - cop.material.at(number2).seg.first.y);
                        vertical_vec2.y = cop.material.at(number2).seg.second.x - cop.material.at(number2).seg.first.x;
                    }
                    newelement.arc.q.x = newelement.arc.c.x + vertical_vec2.x * fraction;
                    newelement.arc.q.y = newelement.arc.c.y + vertical_vec2.y * fraction;
                    copbuf.material.at(present).seg.first = newelement.arc.q;
                    newelement.arc.direction = 0; // CW
                    copbuf.material.insert(copbuf.material.begin() + present, newelement);
                    ++present;
                    ++before;
                }
            }
            else if ((copbuf.material.at(before).is_arc == 0 && copbuf.material.at(present).is_arc == 1)) {
                point inn1((copbuf.material.at(present).arc.p.x - copbuf.material.at(present).arc.c.x), (copbuf.material.at(present).arc.p.y - copbuf.material.at(present).arc.c.y)), inn2((copbuf.material.at(before).seg.second.x - copbuf.material.at(before).seg.first.x), (copbuf.material.at(before).seg.second.y - copbuf.material.at(before).seg.first.y));
                if (copbuf.material.at(present).arc.in_angle(cop.material.at(number2).arc.start_angle()) && !((db::equal(inn1.x, 0) || db::equal(inn2.x, 0)) && (db::equal(inn1.y, 0) || db::equal(inn2.y, 0)))) {
                    double radius = copbuf.material.at(present).arc.radius();
                    copbuf.material.at(present).arc.p.x = copbuf.material.at(present).arc.c.x + radius * cos(cop.material.at(number2).arc.start_angle()* M_PI / 180);
                    copbuf.material.at(present).arc.p.y = copbuf.material.at(present).arc.c.y + radius * sin(cop.material.at(number2).arc.start_angle() * M_PI / 180);
                    newelement.arc.c = cop.material.at(number2).arc.p;
                    newelement.arc.q = copbuf.material.at(present).arc.p;
                    double fraction = gm::distance(cop.material.at(number1).seg.second, copbuf.material.at(before).seg) / copbuf.material.at(before).seg.length();
                    point vec1(copbuf.material.at(before).seg.second.x - copbuf.material.at(before).seg.first.x, copbuf.material.at(before).seg.second.y - copbuf.material.at(before).seg.first.y);
                    point vertical_vec1(vec1.y, -1 * vec1.x);
                    if (db::less(gm::crossed_product(vec1, vertical_vec1), 0)) {
                        vertical_vec1.x = -1 * vec1.y;
                        vertical_vec1.y = vec1.x;
                    }
                    newelement.arc.p.x = newelement.arc.c.x + vertical_vec1.x * fraction;
                    newelement.arc.p.y = newelement.arc.c.y + vertical_vec1.y * fraction;
                    copbuf.material.at(before).seg.second = newelement.arc.p;
                    newelement.arc.direction = 0; // CW
                    copbuf.material.insert(copbuf.material.begin() + present, newelement);
                    ++present;
                    ++before;
                }
            }
            else if ((copbuf.material.at(before).is_arc == 1 && copbuf.material.at(present).is_arc == 0)) {
                point inn1((copbuf.material.at(before).arc.q.x - copbuf.material.at(before).arc.c.x), (copbuf.material.at(before).arc.q.y - copbuf.material.at(before).arc.c.y)), inn2((copbuf.material.at(present).seg.second.x - copbuf.material.at(present).seg.first.x), (copbuf.material.at(present).seg.second.y - copbuf.material.at(present).seg.first.y));
                if (copbuf.material.at(before).arc.in_angle(cop.material.at(number1).arc.end_angle()) && !((db::equal(inn1.x, 0) || db::equal(inn2.x, 0)) && (db::equal(inn1.y, 0) || db::equal(inn2.y, 0)))) {
                    double radius = copbuf.material.at(before).arc.radius();
                    copbuf.material.at(before).arc.q.x = copbuf.material.at(before).arc.c.x + radius * cos(cop.material.at(number1).arc.end_angle() * M_PI / 180);
                    copbuf.material.at(before).arc.q.y = copbuf.material.at(before).arc.c.y + radius * sin(cop.material.at(number1).arc.end_angle() * M_PI / 180);
                    newelement.arc.c = cop.material.at(number1).arc.q;
                    newelement.arc.p = copbuf.material.at(before).arc.q;
                    double fraction = gm::distance(cop.material.at(number2).seg.first, copbuf.material.at(present).seg) / copbuf.material.at(present).seg.length();
                    point vec2(copbuf.material.at(present).seg.second.x - copbuf.material.at(present).seg.first.x, copbuf.material.at(present).seg.second.y - copbuf.material.at(present).seg.first.y);
                    point vertical_vec2(vec2.y, -1 * vec2.x);
                    if (db::less(gm::crossed_product(vec2, vertical_vec2), 0)) {
                        vertical_vec2.x = -1 * vec2.y;
                        vertical_vec2.y = vec2.x;
                    }
                    newelement.arc.q.x = newelement.arc.c.x + vertical_vec2.x * fraction;
                    newelement.arc.q.y = newelement.arc.c.y + vertical_vec2.y * fraction;
                    copbuf.material.at(present).seg.first = newelement.arc.q;
                    newelement.arc.direction = 0; // CW
                    copbuf.material.insert(copbuf.material.begin() + present, newelement);
                    ++present;
                    ++before;
                }
            }
            else {
                point vec1((copbuf.material.at(before).arc.q.y - copbuf.material.at(before).arc.c.y), (-1 * copbuf.material.at(before).arc.q.x + copbuf.material.at(before).arc.c.x)), vec2((copbuf.material.at(present).arc.p.y - copbuf.material.at(present).arc.c.y), (-1 * copbuf.material.at(present).arc.p.x + copbuf.material.at(present).arc.c.x));
                if (copbuf.material.at(before).arc.in_angle(cop.material.at(number1).arc.end_angle()) && copbuf.material.at(present).arc.in_angle(cop.material.at(number2).arc.start_angle()) && !db::equal(gm::crossed_product(vec1, vec2), 0)) {
                    double bradius = copbuf.material.at(before).arc.radius(), pradius = copbuf.material.at(present).arc.radius();
                    copbuf.material.at(before).arc.q.x = copbuf.material.at(before).arc.c.x + bradius * cos(cop.material.at(number1).arc.end_angle() * M_PI / 180);
                    copbuf.material.at(before).arc.q.y = copbuf.material.at(before).arc.c.y + bradius * sin(cop.material.at(number1).arc.end_angle() * M_PI / 180);
                    copbuf.material.at(present).arc.p.x = copbuf.material.at(present).arc.c.x + pradius * cos(cop.material.at(number2).arc.start_angle() * M_PI / 180);
                    copbuf.material.at(present).arc.p.y = copbuf.material.at(present).arc.c.y + pradius * sin(cop.material.at(number2).arc.start_angle() * M_PI / 180);
                    newelement.arc.c = cop.material.at(number1).arc.q;
                    newelement.arc.p = copbuf.material.at(before).arc.q;
                    newelement.arc.q = copbuf.material.at(present).arc.p;
                    segment seg1(cop.material.at(before).arc.c, newelement.arc.p), seg2(cop.material.at(present).arc.c, newelement.arc.q);
                    bool intersection = seg1.intersects(seg2);
                    if ((copbuf.material.at(before).arc.direction == 0 && copbuf.material.at(present).arc.direction == 1) && (copbuf.material.at(before).arc.direction == 1 && copbuf.material.at(present).arc.direction == 0))
                        newelement.arc.direction = 0;
                    else if (copbuf.material.at(before).arc.direction == 0 && copbuf.material.at(present).arc.direction == 0) {
                        if (intersection)
                            newelement.arc.direction = 0;
                        else
                            newelement.arc.direction = 1;
                    }
                    else if (copbuf.material.at(before).arc.direction == 1 && copbuf.material.at(present).arc.direction == 1) {
                        if (intersection)
                            newelement.arc.direction = 1;
                        else
                            newelement.arc.direction = 0;
                    }
                    copbuf.material.insert(copbuf.material.begin() + present, newelement);
                    ++present;
                    ++before;
                }
            }
            ++present;
            ++before;
            number1 = number2;
            ++number2;
            finish[0] = false;
            finish[1] = false;

            if (number2 == end)
                break;
            if (present == (copbuf.material.size() - 1) && last == 0)
                last = 1;
            else if (present == copbuf.material.size() && last == 1) {
                before = copbuf.material.size() - 1;
                present = 0;
                number2 = 0;
                last = 2;
            }
            else if (last == 2) {
                before = present - 1;
                number1 = number2 - 1;
            }
        }
    }
    point shifting_point(point p, point q, double d) {
        // p is the starting point, q is the end point, d is the shifting distance,
        // the function find the point that shifted p the given distance in the direction of pq.
        double scale = d / distance(p, q);
        return point{ (q.x - p.x) * scale + p.x , (q.y - p.y) * scale + p.y };
    }
    double distance(point p, Arc arc) {
        if (arc.on_arc(p))
            return 0;
        vector<double> dis;
        dis.push_back(distance(p, arc.p));
        dis.push_back(distance(p, arc.q));
        if (arc.in_angle(p)) {
            dis.push_back(distance(p, arc.c) - arc.radius());
        }
        return *std::min_element(dis.begin(), dis.end());
    }
    double distance(point p, segment seg) {
        segment result;
        closest_points(p, seg, result);
        return result.length();
    }
    double distance(point p, point q) {
        return p.distance(q);
    }
    double distance(segment seg1, segment seg2) {
        double a, b, c, aa, bb, cc;//ax+by+c=0 &aax+bby+cc=0
        seg_translate(seg1, a, b, c, 0);
        seg_translate(seg2, aa, bb, cc, 0);
        double d = a * bb - aa * b;
        point p((b * cc - bb * c) / d, (aa * c - a * cc) / d);
        if (seg1.on_segment(p) && seg2.on_segment(p))
            return 0;
        //make the intersection using formula
        double dis[4];
        segment receiver;
        closest_points(seg1.first, seg2, receiver);
        dis[0] = receiver.length();
        closest_points(seg1.second, seg2, receiver);
        dis[1] = receiver.length();
        closest_points(seg2.first, seg1, receiver);
        dis[2] = receiver.length();
        closest_points(seg2.second, seg1, receiver);
        dis[3] = receiver.length();
        double min = dis[0];
        return *std::min_element(dis, dis + 4);
    }
    double distance(point p, element ele) {
        if (ele.is_arc) {
            return distance(p, ele.arc);
        }
        else {
            return distance(p, ele.seg);
        }
    }
    double crossed_product(point v1, point v2) {
        //use a point(x,y) as a vector(x,y)
        return v1.x * v2.y - v1.y * v2.x;
    }
    int orientation(point p, point q, point r)
    {
        // To find orientation of ordered triplet (p, q, r).
        // The function returns following values
        // 0 --> p, q and r are collinear
        // 1 --> Clockwise
        // 2 --> Counterclockwise
        int val = (q.y - p.y) * (r.x - q.x) -
            (q.x - p.x) * (r.y - q.y);

        if (val == 0) return 0;  // collinear

        return (val > 0) ? 1 : 2; // clock or counterclock wise
    }
    double element_distance(element e1, element e2) {
        if (intersects(e1, e2)) {
            return 0;
        }
        // finding distance between two element
        // if both elements are line
        if (!e1.is_arc && !e2.is_arc) {
            return distance(e1.seg, e2.seg);
        }
        // if either one is arc, the other is line
        else if ((!e1.is_arc && e2.is_arc) || (e1.is_arc && !e2.is_arc)) {
            Arc arc;
            segment seg;
            if (e1.is_arc) { // e1 = arc, e2 = line
                arc = e1.arc;
                seg = e2.seg;
            }
            else { // e1 = line, e2 = arc
                arc = e2.arc;
                seg = e1.seg;
            }
            // find the closest path to line from the arc's center
            segment closest_path;
            closest_points(arc.c, seg, closest_path);
            // if the closest path lies in the angle interval, then it's the valid distance of the two elment
            // or its distance is one of the distance of two endpoint of the arc
            vector<double> dis;
            double r = arc.radius();
            if (arc.in_angle(closest_path.second) && !collinear(closest_path.first, closest_path.second, arc.c)) {
                dis.push_back(abs(closest_path.length() - r));
            }
            dis.push_back(distance(arc.p, seg));
            dis.push_back(distance(arc.q, seg));
            if (arc.in_angle(seg.first)) {
                dis.push_back(abs(distance(arc.c, seg.first) - r));
            }
            if (arc.in_angle(seg.second)) {
                dis.push_back(abs(distance(arc.c, seg.second) - r));
            }
            dis.push_back(distance(arc.p, seg.first));
            dis.push_back(distance(arc.q, seg.second));
            return *std::min_element(dis.begin(), dis.end());
        }

        // if both elements are arc
        if (e1.is_arc && e2.is_arc) {
            // if the line of centers intersecs both arcs,
            // then their distance is abs(r1 + r2 - |cc|)
            double r1 = distance(e1.arc.p, e1.arc.c), r2 = distance(e2.arc.p, e2.arc.c);
            vector<double> dis;
            if (e1.arc.in_angle(e2.arc.c) && e2.arc.in_angle(e1.arc.c)) {
                dis.push_back(abs(r1 + r2 - distance(e1.arc.c, e2.arc.c)));
            }
            // the second case is when the line of centers intersecs only one of the arcs,
            // then the distance is abs(|r1 - r2| - |cc|)
            if ((e1.arc.in_angle(e2.arc.c) && e2.arc.in_across_angle(e1.arc.c))) {
                dis.push_back(abs(r1 - r2 - distance(e1.arc.c, e2.arc.c)));
            }
            if (e1.arc.in_across_angle(e2.arc.c) && e2.arc.in_angle(e1.arc.c)) {
                dis.push_back(abs(r2 - r1 - distance(e1.arc.c, e2.arc.c)));
            }
            // otherwise the distance occurs on the endpoint of either arc
            bool pass_either_arc = false;
            // if the line pass the arc, add into the vector
            // return the min value of the result vector
            if (e2.arc.in_angle(e1.arc.p)) { // p1 c2
                dis.push_back(abs(distance(e1.arc.p, e2.arc.c) - r2));
                pass_either_arc = true;
            }
            if (e2.arc.in_angle(e1.arc.q)) { // q1 c2
                dis.push_back(abs(distance(e1.arc.q, e2.arc.c) - r2));
                pass_either_arc = true;
            }
            if (e1.arc.in_angle(e2.arc.p)) { // p2 c1
                dis.push_back(abs(distance(e2.arc.p, e1.arc.c) - r1));
                pass_either_arc = true;
            }
            if (e1.arc.in_angle(e2.arc.q)) { // q2 c1
                dis.push_back(abs(distance(e2.arc.q, e1.arc.c) - r1));
                pass_either_arc = true;
            }
            if (!pass_either_arc) { // if none of the lines pass the arcs
                dis.push_back(distance(e1.arc.p, e2.arc.q));
                dis.push_back(distance(e1.arc.p, e2.arc.p));
                dis.push_back(distance(e1.arc.q, e2.arc.p));
                dis.push_back(distance(e1.arc.q, e2.arc.q));
            }
            return *min_element(dis.begin(), dis.end());
        }
    }
    bool intersects(element e1, element e2) {
        if (!e1.is_arc && !e2.is_arc) {
            return (e1.seg.intersects(e2.seg));
        }
        else if ((!e1.is_arc && e2.is_arc) || (e1.is_arc && !e2.is_arc)) {
            // declaration
            Arc arc;
            segment seg;
            if (e1.is_arc) {
                arc = e1.arc;
                seg = e2.seg;
            }
            else {
                arc = e2.arc;
                seg = e1.seg;
            }
            // the distance of seg and the arc's center is bigger than radius
            if (gm::distance(arc.c, seg) > arc.radius()) {
                // there's no intersection between line and arc
                return false;
            }
            // the entire seg is in the arc's circle
            else if (gm::distance(arc.c, seg) < arc.radius() && gm::distance(seg.first, arc.c) < arc.radius() && gm::distance(seg.second, arc.c) < arc.radius()) {
                return false;
            }
            // the distance of seg and the arc's center is less than or equal to radius
            else {
                point p1, p2;
                find_ArcLine_intersection(arc, seg, p1, p2);
                if (seg.on_segment(p1) && arc.on_arc(p1))
                    return true;
                if (seg.on_segment(p2) && arc.on_arc(p2))
                    return true;
                return false;
            }
        }
        else if (e1.is_arc && e2.is_arc) {
            Arc arc1 = e1.arc, arc2 = e2.arc;
            double center_distance = distance(arc1.c, arc2.c),
                rsum = arc1.radius() + arc2.radius(),
                rdiff = abs(arc1.radius() - arc2.radius());
            // if the centers are the same
            if (arc1.c == arc2.c && arc1.radius() == arc2.radius()) {
                if (arc2.on_arc(arc1.p) || arc2.on_arc(arc1.q))
                    return true;
                else
                    return false;
            }
            // if the distance of two center is more than sum of radius
            // or less than difference of two radius 
            else if (center_distance > rsum || center_distance < rdiff) {
                // there's no intersection
                return false;
            }

            // if the center distance is equal to rsum or rdiff
            else if (center_distance == rsum) {
                // there's exactly one intersection
                // intersection is at direction of c1 -> c2 and on the arc
                point intersection = shifting_point(arc1.c, arc2.c, arc1.radius());
                // if the intersection point is on both arcs
                if (arc1.on_arc(intersection) && arc2.on_arc(intersection))
                    return true;
                else
                    return false;
            }

            else if (center_distance == rdiff) {
                // there's exactly one intersection
                // if r1 > r2, intersection is at direction of c1 -> c2 and on the arc
                point intersection;
                if (arc1.radius() > arc2.radius())
                    intersection = shifting_point(arc1.c, arc2.c, arc1.radius());
                else
                    intersection = shifting_point(arc2.c, arc1.c, arc2.radius());

                // if the intersection point is on both arcs
                if (arc1.on_arc(intersection) && arc2.on_arc(intersection))
                    return true;
                else
                    return false;
            }

            else {
                // there's two intersections
                point p1, p2;
                find_ArcArc_intersection(e1, e2, p1, p2);
                if (arc1.on_arc(p1) && arc2.on_arc(p1))
                    return true;
                if (arc1.on_arc(p2) && arc2.on_arc(p2))
                    return true;
            }
            return false;
        }
    }
    bool on_segment(point p, point q, point r) {
        return segment(p, r).on_segment(q);
    }
    void closest_points(point p, segment seg, segment& result) {
        // if(p==s||p==f), ans = pp
        if ((db::equal(p.x, seg.first.x) && db::equal(p.y, seg.first.y)) || (db::equal(p.x, seg.second.x) && db::equal(p.y, seg.second.y))) {
            result = segment(p, p);
            return;
        }
        // if p is on the segment, result = pp
        // if p is collinear with seg, result = pf or ps
        if (collinear(p, seg.first, seg.second)) {
            if (seg.on_segment(p)) {
                result = segment(p, p);
                return;
            }
            if (db::less(distance(p, seg.first), distance(p, seg.second))) {
                result = segment(p, seg.first);
                return;
            }
            else {
                result = segment(p, seg.second);
                return;
            }
        }
        // take p's perpendicular point to seg, test if lies on seg
        double a, b, c, aa, bb, cc;
        seg_translate(seg, a, b, c, 0);
        aa = b;
        bb = -a;
        cc = -b * p.x + a * p.y;
        double d = a * bb - aa * b;
        // perpendicular point
        point o = point((b * cc - bb * c) / d, (aa * c - a * cc) / d);
        if (seg.on_segment(o)) {
            result = segment(p, o);
            return;
        }
        else {
            if (db::less(distance(p, seg.first), distance(p, seg.second))) {
                result = segment(p, seg.first);
                return;
            }
            else {
                result = segment(p, seg.second);
                return;
            }
        }
    }
    void create_material(component source, component& receiver, double gap) {
        if (source.material.size() == 1 && source.material.at(0).is_arc == 1) {
            element circle;
            double radius = distance(source.material.at(0).arc.c, source.material.at(0).arc.p) + gap;
            circle.is_arc = 1;
            circle.numbers = 0;
            circle.arc.c = source.material.at(0).arc.c;
            circle.arc.p.x = (source.material.at(0).arc.c.x + radius);
            circle.arc.p.y = (source.material.at(0).arc.c.y);
            circle.arc.q = circle.arc.p;
            receiver.material.push_back(circle);
            return;
        }
        int former = source.material.size() - 1;
        point p, reserve;
        for (int i = 0; i < source.material.size(); i++) {
            reserve = p;
            if (source.material[former].is_arc == 0 && source.material[i].is_arc == 0) {//line line
                double a, b, c, aa, bb, cc;//ax+by+c=0 &aax+bby+cc=0
                seg_translate(source.material[former].seg, a, b, c, gap);
                seg_translate(source.material[i].seg, aa, bb, cc, gap);
                //make the intersection using formula
                double d = a * bb - aa * b;
                p = point((b * cc - bb * c) / d, (aa * c - a * cc) / d);
            }
            else if (source.material[former].is_arc == 0 && source.material[i].is_arc == 1) {//line arc
                p = find_ArcLine_intersection(source.material[i].arc, source.material[former].seg, false, gap);
            }
            else if (source.material[former].is_arc == 1 && source.material[i].is_arc == 0) {//arc line
                p = find_ArcLine_intersection(source.material[former].arc, source.material[i].seg, true, gap);
            }
            else if (source.material[former].is_arc == 1 && source.material[i].is_arc == 1) {//arc arc
                p = find_ArcArc_intersection(source.material[former], source.material[i], gap);
            }
            //create new element
            element new_element;
            new_element.numbers = former;
            if (i != 0 && source.material[former].is_arc == 0) {//line
                new_element.is_arc = 0;
                new_element.seg = segment(reserve, p);
                receiver.material.push_back(new_element);
            }
            else if (i != 0 && source.material[former].is_arc == 1) {//arc
                new_element.is_arc = 1;
                new_element.arc.c = source.material[former].arc.c;
                new_element.arc.p = reserve;
                new_element.arc.q = p;
                new_element.arc.direction = source.material[former].arc.direction;
                receiver.material.push_back(new_element);
            }
            former = i;
        }
        //the last point will be the same as the first point
        //create the element between the last two points
        element new_element;
        new_element.numbers = former;
        if (source.material[former].is_arc == 0) { //line
            new_element.is_arc = 0;
            new_element.seg = segment{ receiver.material[former - 1].getEndPoint(), receiver.material[0].getStartPoint() };
        }
        else if (source.material[former].is_arc == 1) { //arc
            new_element.is_arc = 1;
            new_element.arc.c = source.material[former].arc.c;
            new_element.arc.p = receiver.material[former - 1].getEndPoint();
            new_element.arc.q = receiver.material[0].getStartPoint();
            new_element.arc.direction = source.material[former].arc.direction;
        }
        receiver.material.push_back(new_element);
    }
    void seg_translate(segment seg, double& a, double& b, double& c, double gap) {
        //buffered line equation:ax+by+c=0
        a = seg.first.y - seg.second.y;
        b = seg.second.x - seg.first.x;
        if (gap == 0) {
            c = seg.first.x * seg.second.y - seg.second.x * seg.first.y;
            return;
        }
        point midpoint(seg.first.x / 2 + seg.second.x / 2, seg.first.y / 2 + seg.second.y / 2);
        point shiftDir(a / sqrt(a * a + b * b), b / sqrt(a * a + b * b));
        point shift(shiftDir.x * gap, shiftDir.y * gap);
        //check which side is outside the polygon
        //using crossed product
        //if the result is positive,the shift direction is correct

        point v1(seg.second.x - seg.first.x, seg.second.y - seg.first.y);
        point v2(a / sqrt(a * a + b * b), b / sqrt(a * a + b * b));
        if (crossed_product(v1, v2) < 0) {
            shift = point(shift.x * -1, shift.y * -1);
        }
        //the two new points in the new line
        c = (seg.first.x + shift.x) * (seg.second.y + shift.y) - (seg.second.x + shift.x) * (seg.first.y + shift.y);
    }
    point find_ArcLine_intersection(Arc arc, segment seg, bool IsArcLead, double gap)
    {
        double a, b, c;
        seg_translate(seg, a, b, c, gap);
        //IsArcLead=1 =>arc->line
        //IsArcLead=0 =>line->arc
        double radius = 0;
        if (arc.direction == 0) //bigger
            radius = distance(arc.c, arc.p) + gap;
        else //smaller
            radius = distance(arc.c, arc.p) - gap;
        double CenterLineDistance = (abs(a * arc.c.x + b * arc.c.y + c)) / sqrt(a * a + b * b);
        double ShiftingDistance = sqrt(radius * radius - CenterLineDistance * CenterLineDistance);
        //find the point on the line which is nearest to the center
        double ratio = CenterLineDistance / sqrt(a * a + b * b);
        point p0(arc.c.x + a * ratio, arc.c.y + b * ratio);//<a,b>
        point p1(arc.c.x - a * ratio, arc.c.y - b * ratio);//<-a,-b>
        point midpoint;
        double dis = abs(a * arc.c.x + b * arc.c.y + c) / sqrt(a * a + b * b);
        if (db::equal(radius, dis)) {
            if (db::equal(abs(a * p0.x + b * p0.y + c) / sqrt(a * a + b * b), 0))
                return p0;
            else
                return p1;
        }
        //select
        if (db::less(abs(a * p0.x + b * p0.y + c), abs(a * p1.x + b * p1.y + c)))
            midpoint = p0;
        else
            midpoint = p1;
        //find the two intersection points from midpoint toward two direction
        ratio = ShiftingDistance / sqrt(a * a + b * b);
        point rp1, rp2;//may have one or two intersection points
        rp1 = point(midpoint.x + b * ratio, midpoint.y - a * ratio);
        rp2 = point(midpoint.x - b * ratio, midpoint.y + a * ratio);
        //for element_intersection
        if (gap == 0) {
            if (seg.on_segment(rp1))
                return rp1;
            else
                return rp2;
        }
        //for tangent line
        //the distance for line to point
        //
        //return the info
        if (IsArcLead == 0) {//depend on p
            if (db::less_or_equal(distance(rp1, arc.p), distance(rp2, arc.p)))
                return rp1;
            else
                return rp2;
        }
        else if (IsArcLead == 1) {//depend on q
            if (db::less_or_equal(distance(rp1, arc.q), distance(rp2, arc.q)))
                return rp1;
            else
                return rp2;
        }
    }
    point find_ArcArc_intersection(element e1, element e2, double gap) {
        // find the intersection of two arc after expansion
        // the elements must be in CW order.
        if (e1.arc.c == e2.arc.c) {
            return e1.arc.q;
        }
        double r1, r2, d, a, h;
        // if the arc is in CW direction, then the new radius has to add the gap distance,
        // if it's in CCW direction then otherwise.
        if (e1.arc.direction)
            r1 = distance(e1.arc.p, e1.arc.c) - gap;
        else
            r1 = distance(e1.arc.p, e1.arc.c) + gap;
        if (e2.arc.direction)
            r2 = distance(e2.arc.p, e2.arc.c) - gap;
        else
            r2 = distance(e2.arc.p, e2.arc.c) + gap;

        // solving two points coordinate
        d = e1.arc.c.distance(e2.arc.c);
        a = (r1 * r1 - r2 * r2 + d * d) / (2 * d);
        h = sqrt(r1 * r1 - a * a);

        point P2 = point{ e1.arc.c.x + a * (e2.arc.c.x - e1.arc.c.x) / d, e1.arc.c.y + a * (e2.arc.c.y - e1.arc.c.y) / d };

        // intersection points are P3, P4
        double x3, y3, x4, y4;
        x3 = P2.x + h * (e2.arc.c.y - e1.arc.c.y) / d;
        y3 = P2.y - h * (e2.arc.c.x - e1.arc.c.x) / d;
        x4 = P2.x - h * (e2.arc.c.y - e1.arc.c.y) / d;
        y4 = P2.y + h * (e2.arc.c.x - e1.arc.c.x) / d;

        // case of tangent circles
        if (db::equal(e1.arc.p.distance(e1.arc.c) + e2.arc.p.distance(e2.arc.c), d)) { // distance of centers == sum of two radius
            if (!e1.arc.direction && !e2.arc.direction) { // if both arcs are CW
                // calculate the cross product of p1P3 and p1q1 to find if P3 locates within the graph
                point v1(x3 - e1.arc.p.x, y3 - e1.arc.p.y);
                point v2(e1.arc.q.x - e1.arc.p.x, e1.arc.q.y - e1.arc.p.y);
                if (crossed_product(v1, v2) > 0) // is not within the graph
                    return point(x3, y3); // returns P3
                else
                    return point(x4, y4); // P3 is in the graph, returns P4
            }
            else {
                // if two arcs are in different direction
                // then there's only one intersection point
                Arc arc1, arc2; // arc1 is in CW, arc2 is in CCW
                if (e2.arc.direction) {
                    arc1 = e1.arc;
                    arc2 = e2.arc;
                }
                else {
                    arc1 = e2.arc;
                    arc2 = e1.arc;
                }
                // shifting on the line of centers for distance of arc1's radius + gap.
                return shifting_point(arc1.c, arc2.c, arc1.c.distance(arc1.p) + gap);
            }
        }
        // return the point in the angle of arc1
        if (gap == 0) {
            if (e1.arc.in_angle(point(x3, y3)))
                return point(x3, y3);
            else
                return point(x4, y4);
        }
        // return the closer point to the original intersection point (case of buffers)
        else {//CCW CCW
            if (e1.arc.q.distance(point(x3, y3)) < e1.arc.q.distance(point(x4, y4)))
                return point(x3, y3);
            else
                return point(x4, y4);
        }
    }
    void find_ArcLine_intersection(Arc arc, segment seg, point& p1, point& p2) {
        double a, b, c;
        seg_translate(seg, a, b, c, 0);
        double radius = arc.radius();
        double CenterLineDistance = (abs(a * arc.c.x + b * arc.c.y + c)) / sqrt(a * a + b * b);
        double ShiftingDistance = sqrt(radius * radius - CenterLineDistance * CenterLineDistance);
        //find the point on the line which is nearest to the center
        double ratio = CenterLineDistance / sqrt(a * a + b * b);
        point P0 = point(arc.c.x + a * ratio, arc.c.y + b * ratio);//<a,b>
        point P1 = point(arc.c.x - a * ratio, arc.c.y - b * ratio);//<-a,-b>
        point midpoint;
        double dis = abs(a * arc.c.x + b * arc.c.y + c) / sqrt(a * a + b * b);
        // line is tangent to circle case
        if (db::equal(radius, dis)) {
            if (db::equal(abs(a * P0.x + b * P0.y + c) / sqrt(a * a + b * b), 0))
                p1 = P0;
            else
                p1 = P1;
            p2 = p1;
        }
        //select
        if (db::less(abs(a * P0.x + b * P0.y + c), abs(a * P1.x + b * P1.y + c)))
            midpoint = P0;
        else
            midpoint = P1;
        //find the two intersection points from midpoint toward two direction
        ratio = ShiftingDistance / sqrt(a * a + b * b);
        p1 = point(midpoint.x + b * ratio, midpoint.y - a * ratio);
        p2 = point(midpoint.x - b * ratio, midpoint.y + a * ratio);
    }
    void find_ArcArc_intersection(element e1, element e2, point& p1, point& p2) {
        // the function is intended to find the intersection of two " circle "
        // there's no fool proof to prevent the function from falling into zero interesection situation
        double r1 = e1.arc.radius(), r2 = e2.arc.radius(), d, a, h;
        // solving two points coordinate
        d = e1.arc.c.distance(e2.arc.c);
        a = (r1 * r1 - r2 * r2 + d * d) / (2 * d);
        h = sqrt(r1 * r1 - a * a);
        point P2 = point{ e1.arc.c.x + a * (e2.arc.c.x - e1.arc.c.x) / d, e1.arc.c.y + a * (e2.arc.c.y - e1.arc.c.y) / d };
        // intersection points are P3, P4
        double x3, y3, x4, y4;
        x3 = P2.x + h * (e2.arc.c.y - e1.arc.c.y) / d;
        y3 = P2.y - h * (e2.arc.c.x - e1.arc.c.x) / d;
        x4 = P2.x - h * (e2.arc.c.y - e1.arc.c.y) / d;
        y4 = P2.y + h * (e2.arc.c.x - e1.arc.c.x) / d;
        p1 = point(x3, y3);
        p2 = point(x4, y4);
    }
    bool collinear(point a, point b, point c) {
        //the function is used to check if a,b,c are collinear
        return a.collinear(b, c);
    }
    point element_intersection(element e1, element e2) {
        point p;
        if (e1.is_arc == 0 && e2.is_arc == 0) {//line line
            double a, b, c, aa, bb, cc;//ax+by+c=0 &aax+bby+cc=0
            seg_translate(e1.seg, a, b, c, 0);
            seg_translate(e2.seg, aa, bb, cc, 0);
            //make the intersection using formula
            if (db::equal(aa, 0) && db::equal(a, 0) && db::equal(b / bb, c / cc)) { //horizontal collinear line
                p = e1.seg.second;
            }
            else if (db::equal(aa, 0) && db::equal(a, 0) && !db::equal(b / bb, c / cc)) {
                cout << "parallel line(horizontal noncollinear line) has no intersection!!!(element_intersection())" << endl;
            }
            else if (db::equal(bb, 0) && db::equal(b, 0) && db::equal(a / aa, c / cc)) {//vertical collinear line
                p = e1.seg.second;
            }
            else if (db::equal(bb, 0) && db::equal(b, 0) && !db::equal(a / aa, c / cc)) {
                cout << "parallel line(vertical noncollinear line) has no intersection!!!(element_intersection())" << endl;
            }
            else if (db::equal(a / aa, b / bb) && db::equal(a / aa, c / cc)) { // lines are parellel, collinear
                p = e1.seg.second;
            }
            else if (db::equal(a / aa, b / bb) && !db::equal(a / aa, c / cc)) {//no crossed line;
                cout << "parallel line(arbitrary noncollinear line) has no intersection!!!(element_intersection())" << endl;
            }
            else {
                double d = a * bb - aa * b;
                p = point((b * cc - bb * c) / d, (aa * c - a * cc) / d);
            }
        }
        else if (e1.is_arc == 0 && e2.is_arc == 1) {//line arc
            p = find_ArcLine_intersection(e2.arc, e1.seg, false, 0);
        }
        else if (e1.is_arc == 1 && e2.is_arc == 0) {//arc line
            p = find_ArcLine_intersection(e1.arc, e2.seg, true, 0);
        }
        else if (e1.is_arc == 1 && e2.is_arc == 1) {//arc arc
            p = find_ArcArc_intersection(e1, e2, 0);
        }
        return p;
    }
    void element_intersection(element e1, element e2, vector<point>& receiver) {
        if (!e1.is_arc && !e2.is_arc) {
            if (e1.seg.intersects(e2.seg)) {
                point p;
                double a, b, c, aa, bb, cc;//ax+by+c=0 &aax+bby+cc=0
                seg_translate(e1.seg, a, b, c, 0);
                seg_translate(e2.seg, aa, bb, cc, 0);
                //make the intersection using formula
                if (db::equal(aa, 0) && db::equal(a, 0) && db::equal(b / bb, c / cc)) { //horizontal collinear line
                    p = e1.seg.second;
                }
                else if (db::equal(aa, 0) && db::equal(a, 0) && !db::equal(b / bb, c / cc)) {
                    cout << "parallel line(horizontal noncollinear line) has no intersection!!!(element_intersection())" << endl;
                }
                else if (db::equal(bb, 0) && db::equal(b, 0) && db::equal(a / aa, c / cc)) {//vertical collinear line
                    p = e1.seg.second;
                }
                else if (db::equal(bb, 0) && db::equal(b, 0) && !db::equal(a / aa, c / cc)) {
                    cout << "parallel line(vertical noncollinear line) has no intersection!!!(element_intersection())" << endl;
                }
                else if (db::equal(a / aa, b / bb) && db::equal(a / aa, c / cc)) { // lines are parellel, collinear
                    p = e1.seg.second;
                }
                else if (db::equal(a / aa, b / bb) && !db::equal(a / aa, c / cc)) {//no crossed line;
                    cout << "parallel line(arbitrary noncollinear line) has no intersection!!!(element_intersection())" << endl;
                }
                else {
                    double d = a * bb - aa * b;
                    p = point((b * cc - bb * c) / d, (aa * c - a * cc) / d);
                }
                receiver.push_back(p);
            }
            return;
        }
        else if ((!e1.is_arc && e2.is_arc) || (e1.is_arc && !e2.is_arc)) {
            // declaration
            Arc arc;
            segment seg;
            if (e1.is_arc) {
                arc = e1.arc;
                seg = e2.seg;
            }
            else {
                arc = e2.arc;
                seg = e1.seg;
            }
            // the distance of seg and the arc's center is bigger than radius
            if (distance(arc.c, seg) > arc.radius()) {
                // there's no intersection between line and arc
                return;
            }
            // the entire seg is in the arc's circle
            else if (distance(arc.c, seg) < arc.radius() && gm::distance(seg.first, arc.c) < arc.radius() &&
                gm::distance(seg.second, arc.c) < arc.radius()) {
                return;
            }
            // the distance of seg and the arc's center is less than or equal to radius
            else {
                point p1, p2;
                find_ArcLine_intersection(arc, seg, p1, p2);
                if (seg.on_segment(p1) && arc.on_arc(p1)) {
                    receiver.push_back(p1);
                }
                if (seg.on_segment(p2) && arc.on_arc(p2)) {
                    receiver.push_back(p2);
                }
                return;
            }
        }
        else if (e1.is_arc && e2.is_arc) {
            Arc arc1 = e1.arc, arc2 = e2.arc;
            double center_distance = distance(arc1.c, arc2.c),
                rsum = arc1.radius() + arc2.radius(),
                rdiff = abs(arc1.radius() - arc2.radius());
            // if the centers are the same
            if (arc1.c == arc2.c && arc1.radius() == arc2.radius()) {
                if (arc2.on_arc(arc1.p) || arc2.on_arc(arc1.q)) {
                    receiver.push_back(arc1.q);
                }
                return;
            }
            // if the distance of two center is more than sum of radius
            // or less than difference of two radius 
            else if (center_distance > rsum || center_distance < rdiff) {
                // there's no intersection
                return;
            }
            // if the center distance is equal to rsum or rdiff
            else if (center_distance == rsum) {
                // there's exactly one intersection
                // intersection is at direction of c1 -> c2 and on the arc
                point intersection = shifting_point(arc1.c, arc2.c, arc1.radius());
                // if the intersection point is on both arcs
                if (arc1.on_arc(intersection) && arc2.on_arc(intersection))
                    receiver.push_back(intersection);
                return;
            }
            else if (center_distance == rdiff) {
                // there's exactly one intersection
                // if r1 > r2, intersection is at direction of c1 -> c2 and on the arc
                point intersection;
                if (arc1.radius() > arc2.radius())
                    intersection = shifting_point(arc1.c, arc2.c, arc1.radius());
                else
                    intersection = shifting_point(arc2.c, arc1.c, arc2.radius());

                // if the intersection point is on both arcs
                if (arc1.on_arc(intersection) && arc2.on_arc(intersection))
                    receiver.push_back(intersection);
                return;
            }

            else {
                // there's two intersections
                point p1, p2;
                find_ArcArc_intersection(e1, e2, p1, p2);
                if (arc1.on_arc(p1) && arc2.on_arc(p1))
                    receiver.push_back(p1);
                if (arc1.on_arc(p2) && arc2.on_arc(p2))
                    receiver.push_back(p2);
                return;
            }
        }
    }
}

namespace io {
    void get_data(ifstream& infile, double& a, double& c, double& s);
    void take_point(string inputstr, struct component& comp);
    void get_assembly_location(ifstream& infile, component& ass, int& arc_number, int& line_number);
    void get_copper_location(ifstream& infile, vector <component>& coppers);
    void silkscreen_output(vector<component> comp, string file_name);//transform the component to the rating file format

    void get_data(ifstream& infile, double& a, double& c, double& s) {
        string buffer;
        infile >> buffer;
        a = stod(buffer.substr(buffer.find(",") + 1));
        infile >> buffer;
        c = stod(buffer.substr(buffer.find(",") + 1));
        infile >> buffer;
        s = stod(buffer.substr(buffer.find(",") + 1));
    }
    void take_point(string inputstr, component& comp) {
        // bool first_graph is the aruement that determine whether its the first graph,
        // which has to input two points into the multipolygon.
        struct element new_element;
        double x1, y1, x2, y2;
        string type; // line or arc
        // taking the graph type ( line or arc )
        if (inputstr[0] == 'a')  new_element.is_arc = true;
        else new_element.is_arc = false;
        inputstr = inputstr.substr(inputstr.find(",") + 1); // delete type in inputstr

        // take starting point of the graph
        x1 = stod(inputstr.substr(0, inputstr.find(',')));
        inputstr = inputstr.substr(inputstr.find(",") + 1); // delete x1 in inputstr
        y1 = stod(inputstr.substr(0, inputstr.find(',')));
        inputstr = inputstr.substr(inputstr.find(",") + 1); // delete y1 in inputstr
        // take end point of the graph
        x2 = stod(inputstr.substr(0, inputstr.find(',')));
        inputstr = inputstr.substr(inputstr.find(",") + 1); // delete x2 in inputstr
        y2 = stod(inputstr.substr(0, inputstr.find(',')));
        inputstr = inputstr.substr(inputstr.find(",") + 1); // delete y2 in inputstr
        // if the graph is an arc, add the arc into assembly_arc
        if (new_element.is_arc) {
            double xc, yc;
            xc = stod(inputstr.substr(0, inputstr.find(',')));
            inputstr = inputstr.substr(inputstr.find(",") + 1);
            yc = stod(inputstr.substr(0, inputstr.find(',')));
            inputstr = inputstr.substr(inputstr.find(",") + 1);
            // direction is 1 if the arc is counterclockwise,
            // is 0 if otherwise.
            if (inputstr == "CCW") {
                new_element.arc.direction = 1;
            }
            else
                new_element.arc.direction = 0;

            // create p, q and c(center)
            new_element.arc.p = point(x1, y1);
            new_element.arc.q = point(x2, y2);
            new_element.arc.c = point(xc, yc);
        }
        else {
            // if the graph is a segment
            new_element.seg = segment{ point(x1, y1) , point(x2, y2) };
        }

        comp.material.push_back(new_element);

        //end of the function
    }
    void get_assembly_location(ifstream& infile, struct component& ass, int& arc_number, int& line_number) {
        //preset
        arc_number = 0;
        line_number = 0;
        string buffer;
        infile >> buffer;
        infile >> buffer;

        while (buffer != "copper") {
            if (buffer[0] == 'a')
                arc_number++;
            else if (buffer[0] == 'l')
                line_number++;
            take_point(buffer, ass);
            if (infile.peek() == EOF)
                break;
            infile >> buffer;
        }
        // reverse the point if the points are ccw

        if (ass.IsCcwSequence()) {//turn to "CW" in element sequence
            ass.reverse_material();
        }
    }
    void get_copper_location(ifstream& infile, vector <component>& coppers) {
        string buffer;
        struct component new_comp;

        while (infile.peek() != EOF) {
            infile >> buffer;
            while (buffer != "copper" && infile.peek() != EOF) {
                take_point(buffer, new_comp);
                infile >> buffer;
            }
            if (new_comp.IsCcwSequence()) {
                new_comp.reverse_material();
            }
            coppers.push_back(new_comp);
            new_comp.material.clear();
        }
    }
    void silkscreen_output(vector<component> silkscreen, string file_name) {
        //the function will detect the discontinuity and add the word "silkscreen"
        ofstream outfile(file_name);
        for (int i = 0; i < silkscreen.size(); i++) {
            outfile << "silkscreen" << endl;
            for (int j = 0; j < silkscreen[i].material.size(); j++) {
                outfile << silkscreen[i].material[j].info() << endl;
            }
        }
    }
}