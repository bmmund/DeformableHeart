#include "acm.hpp"
#include <algorithm>
#include <assert.h>
ACM::ACM()
{

}

void ACM::add(const CMap &cm)
{
    cm_list.push_back(cm);
}

std::vector<CMap>& ACM::connectivityMaps()
{
    return cm_list;
}

void ACM::clear()
{
    cm_list.clear();
    v_list.clear();
}

CMapHandle ACM::addCmap()
{
    CMap cm;
    cm.idx = cm_list.size();

    // allocate space
    std::vector<VertexHandle> v(2, -1);
    cm.cm.resize(2, v);

    cm_list.push_back(cm);
    return cm_list.back().idx;
}

VertexHandle ACM::addVertex()
{
    Vertex vert;
    return addVertex(vert);
}

VertexHandle ACM::addVertex(Vertex new_vertex)
{
    new_vertex.idx = v_list.size();
    v_list.push_back(new_vertex);
    return v_list.back().idx;
}

Vertex* ACM::getVertex(VertexHandle idx)
{
    return &v_list.at(idx);
}

CMap* ACM::getCMap(CMapHandle idx)
{
    return &cm_list.at(idx);
}

void ACM::addVertsToCMap(CMapHandle cm_idx,
                         VertexHandle vh00,
                         VertexHandle vh01,
                         VertexHandle vh10,
                         VertexHandle vh11)
{
    CMap& cm(cm_list.at(cm_idx));
    cm.cm[0][0] = vh00;
    cm.cm[0][1] = vh01;
    cm.cm[1][0] = vh10;
    cm.cm[1][1] = vh11;
    // update v_cm_map
    std::vector<VertexHandle> vhs = {vh00, vh01, vh10, vh11};
    updateVertexCMapMap(vhs, cm.idx);
}

void ACM::setFaceColour(CMapHandle cm_idx, glm::vec3 colour)
{
    cm_list.at(cm_idx).colour = colour;
}

std::vector<std::array<VertexHandle, 3>> ACM::getCMapFaces(CMapHandle cm_idx)
{
    std::vector<std::array<VertexHandle, 3>> faces;
    CMap* cmap = getCMap(cm_idx);
    for(int i = 0; i < cmap->cm.size()-1; i+=cmap->vectorScale)
    {
        for(int j = 0; j < cmap->cm.at(i).size()-1; j+=cmap->vectorScale)
        {
            std::array<VertexHandle, 3> verts;
            // left triangle
            verts[0] = cmap->cm.at(i).at(j);
            verts[1] = cmap->cm.at(i+cmap->vectorScale).at(j);
            verts[2] = cmap->cm.at(i).at(j+cmap->vectorScale);
            faces.push_back(verts);
            // right triangle
            verts[0] = cmap->cm.at(i).at(j+cmap->vectorScale);
            verts[1] = cmap->cm.at(i+cmap->vectorScale).at(j);
            verts[2] = cmap->cm.at(i+cmap->vectorScale).at(j+cmap->vectorScale);
            faces.push_back(verts);
        }
    }
    return faces;
}

void ACM::updateCMapNeighbours()
{
    for(auto& cm : cm_list)
    {
        VertexHandle vh00, vh01, vh10, vh11;
        CMapNeighbour cm_neighbour;
        std::vector<CMapHandle> cmhs;
        vh00 = cm.cm[0][0];
        vh01 = cm.cm[0][1];
        vh10 = cm.cm[1][0];
        vh11 = cm.cm[1][1];

        // edge vh00 vh10 - bottom edge
        cm_neighbour = getCommanCMap(cm.idx, CMapIndex(0,0), CMapIndex(1,0));
        assert(cm_neighbour.n == 0);
        cm.boundaryCMs[cm_neighbour.n] = cm_neighbour;

        // edge vh10 vh11 - right edge
        cm_neighbour = getCommanCMap(cm.idx, CMapIndex(1,0), CMapIndex(1,1));
        assert(cm_neighbour.n == 1);
        cm.boundaryCMs[cm_neighbour.n] = cm_neighbour;

        // edge vh01 vh11 - top edge
        cm_neighbour = getCommanCMap(cm.idx, CMapIndex(0,1), CMapIndex(1,1));
        assert(cm_neighbour.n == 2);
        cm.boundaryCMs[cm_neighbour.n] = cm_neighbour;

        // edge vh00 vh01 - left edge
        cm_neighbour = getCommanCMap(cm.idx, CMapIndex(0,0), CMapIndex(0,1));
        assert(cm_neighbour.n == 3);
        cm.boundaryCMs[cm_neighbour.n] = cm_neighbour;
    }
}

void ACM::updateCMapCorners()
{
    // for each cmap
    for(auto& cm : cm_list)
    {
        // for each corner vertex (ccw starting at 0,0)
        // (0,0) - 0
        cm.cornerVerts[0] = findCMapCornerNeighbours(cm.cm.at(0).at(0));
        // (1,0) - 1
        cm.cornerVerts[1] = findCMapCornerNeighbours(cm.cm.at(1).at(0));
        // (1,1) - 2
        cm.cornerVerts[2] = findCMapCornerNeighbours(cm.cm.at(1).at(1));
        // (0,1) - 3
        cm.cornerVerts[3] = findCMapCornerNeighbours(cm.cm.at(0).at(1));
    }
}

CMapHandle ACM::getCMapForVertex(VertexHandle vh)
{
    CMapHandle cm_idx = -1;
    if(v_cm_map.find(vh) != v_cm_map.end()) // found
    {
        cm_idx = v_cm_map[vh];
    }
    return cm_idx;
}

void ACM::refine()
{
    for(auto& cm : cm_list)
    {
        if(cm.vectorScale > 1)
        {
            cm.vectorScale = std::max((cm.vectorScale / 2), 1);
            continue;
        }

        // Create new temp container for filling
        int origSize = cm.cm.size();
        int newSize = (2*origSize) - 1;
        std::vector<VertexHandle> v(newSize, -1);
        std::vector<std::vector<VertexHandle>> cm_temp(newSize, v);

        // put elements into their correct position
        for(int i = 0; i < cm.cm.size(); i++)
        {
            for(int j = 0; j < cm.cm.at(i).size(); j++)
            {
                cm_temp.at(2*i).at(2*j) = cm.cm.at(i).at(j);
            }
        }

        // replace cm with new bigger cm
        cm.cm = cm_temp;

        for(int i = 0; i < cm.cm.size(); i++)
        {
            for(int j = 0; j < cm.cm.size(); j++)
            {
                if(cm.cm.at(i).at(j) == -1)
                {
                    VertexHandle vert = addVertex();
                    cm.cm.at(i).at(j) = vert;
                    updateVertexCMapMap(vert, cm.idx);
                }
            }
        }
        cm.vectorScale *=2;
    }
}

void ACM::reduceVectorScale()
{
    for(auto& cm : cm_list)
    {
        if(cm.vectorScale > 1)
        {
            cm.vectorScale = std::max((cm.vectorScale / 2), 1);
            continue;
        }
    }
}

void ACM::decompose()
{
    for(auto& cm : cm_list)
    {
        int maxVectorScale = cm.cm.size()-1;
        cm.vectorScale = std::min((cm.vectorScale * 2), maxVectorScale);
    }
}

std::vector<VertexHandle> ACM::getNeighbourhood(VertexHandle vh)
{
    std::vector<VertexHandle> neighbours;
    // find vh in cmap
    CMap* cm;
    CMapIndex location;
    cm = getCMap(getCMapForVertex(vh));
    for(int i = 0; i < cm->cm.size(); i++)
    {
        for(int j = 0; j < cm->cm.at(i).size(); j++)
        {
            if(cm->cm.at(i).at(j) == vh)
            {
                location = CMapIndex(i,j);
                break;
            }
        }
    }
    int max = cm->cm.size()-1;
    CMapIndex v0loc(0,0); // 0,0
    CMapIndex v1loc(max,0); // max,0
    CMapIndex v2loc(max,max); // max,max
    CMapIndex v3loc(0,max); // 0,max

    // on corner
    if(isIndexOnCorner(cm->idx, location))
    {
        neighbours = getCornerNeighbours(cm->idx, location);
    }
    // on edge
    else if((location.x == 0) || (location.y == 0)
            || (location.x == max) || (location.y == max)
            )
    {
        neighbours = getBoundaryNeighbours(cm->idx, location);
    }
    // completely internal
    else
    {
        neighbours = getInternalNeighbours(cm->idx, location);
    }
    return neighbours;
}

std::array<VertexHandle, 4> ACM::getEdgeNeighbours(CMapHandle cmh,
                                                CMapIndex v1,
                                                CMapIndex v2)
{
    std::array<VertexHandle, 4> edgeNeighbours;
    CMap* cm = getCMap(cmh);
    CMap* cm_other;
    int vs, vs_other;
    vs = cm->vectorScale;

    edgeNeighbours.at(0) = cm->cm.at(v1.x).at(v1.y);
    edgeNeighbours.at(1) = cm->cm.at(v2.x).at(v2.y);

    // two remaining verts are in the triangles that share the edge v1v2

    // on boundary edge
    if(isIndexOnBoarder(cmh, v1, v2))
    {
        NeighbourNumber n = getNeighbourNumber(cmh, v1, v2);
        cm_other = getCMap(cm->boundaryCMs.at(n).cmh_pair);
        vs_other = cm_other->vectorScale;
        switch (n) {
            case 0:
                edgeNeighbours.at(2) = cm->cm.at(0).at(vs);
                switch (cm->boundaryCMs.at(n).o) {
                    case CMapOrientation::posi_posj:
                    case CMapOrientation::posj_negi:
                        edgeNeighbours.at(3) = cm_other->cm.at(vs_other).at(0);
                        break;
                    case CMapOrientation::negi_negj:
                    case CMapOrientation::negj_posi:
                        edgeNeighbours.at(3) = cm_other->cm.at(0).at(vs_other);
                        break;
                    default:
                        break;
                }
                break;
            case 1:
                edgeNeighbours.at(2) = cm->cm.at(0).at(vs);
                switch (cm->boundaryCMs.at(n).o) {
                    case CMapOrientation::posi_posj:
                    case CMapOrientation::negj_posi:
                        edgeNeighbours.at(3) = cm_other->cm.at(vs_other).at(0);
                        break;
                    case CMapOrientation::posj_negi:
                    case CMapOrientation::negi_negj:
                        edgeNeighbours.at(3) = cm_other->cm.at(0).at(vs_other);
                        break;
                    default:
                        break;
                }
                break;
            case 2:
                edgeNeighbours.at(2) = cm->cm.at(vs).at(0);
                switch (cm->boundaryCMs.at(n).o) {
                    case CMapOrientation::posi_posj:
                    case CMapOrientation::posj_negi:
                        edgeNeighbours.at(3) = cm_other->cm.at(0).at(vs_other);
                        break;
                    case CMapOrientation::negi_negj:
                    case CMapOrientation::negj_posi:
                        edgeNeighbours.at(3) = cm_other->cm.at(vs_other).at(0);
                        break;
                    default:
                        break;
                }
                break;
            case 3:
                edgeNeighbours.at(2) = cm->cm.at(vs).at(0);
                switch (cm->boundaryCMs.at(n).o) {
                    case CMapOrientation::posi_posj:
                    case CMapOrientation::negj_posi:
                        edgeNeighbours.at(3) = cm_other->cm.at(0).at(vs_other);
                        break;
                    case CMapOrientation::posj_negi:
                    case CMapOrientation::negi_negj:
                        edgeNeighbours.at(3) = cm_other->cm.at(vs_other).at(0);
                        break;
                    default:
                        break;
                }
                break;

            default:
                break;
        }
    }
    // internal
    else
    {
        edgeNeighbours.at(0) = cm->cm.at(v1.x).at(v1.y);
        edgeNeighbours.at(1) = cm->cm.at(v2.x).at(v2.y);
        switch (getEdgeDirection(v1, v2)) {
            case EdgeDirection::vert:
                // make sure v1 is lower
                if(v1.y > v2.y)
                {
                    std::swap(v1, v2);
                }
                edgeNeighbours.at(2) = cm->cm.at(v2.x-vs).at(v2.y);
                edgeNeighbours.at(3) = cm->cm.at(v1.x+vs).at(v1.y);
                break;
            case EdgeDirection::horz:
                // make sure v1 is on left
                if(v1.x > v2.x)
                {
                    std::swap(v1, v2);
                }
                edgeNeighbours.at(2) = cm->cm.at(v1.x).at(v1.y+vs);
                edgeNeighbours.at(3) = cm->cm.at(v2.x).at(v2.y-vs);
                break;
            case EdgeDirection::diag:
                edgeNeighbours.at(2) = cm->cm.at(v1.x).at(v2.y);
                edgeNeighbours.at(3) = cm->cm.at(v2.x).at(v1.y);
                break;

            default:
                break;
        }
    }

    return edgeNeighbours;
}

void ACM::updateVertexCMapMap(const std::vector<VertexHandle>& verts, const CMapHandle cm_idx)
{
    for(const auto& vert : verts)
    {
        updateVertexCMapMap(vert, cm_idx);
    }
}

void ACM::updateVertexCMapMap(VertexHandle vert, const CMapHandle cm_idx)
{
    // this may be unecessary
    if(v_cm_map.find(vert) == v_cm_map.end()) // not found
    {
        //create new entry
        v_cm_map[vert];
    }
    v_cm_map[vert] = cm_idx;
}

CMapNeighbour ACM::getCommanCMap(
                              CMapHandle cmh,
                              CMapIndex vh1_coords,
                              CMapIndex vh2_coords
                              )
{

    CMap* cm_ptr = getCMap(cmh);
    VertexHandle vh1 = cm_ptr->cm.at(vh1_coords.x).at(vh1_coords.y);
    VertexHandle vh2 = cm_ptr->cm.at(vh2_coords.x).at(vh2_coords.y);
    VertexHandle vh1_pair = -1;
    VertexHandle vh2_pair = -1;
    CMapIndex vh1_coords_pair;
    CMapIndex vh2_coords_pair;
    CMapNeighbour cm_pair;
    CMapHandle cmh_pair = -1;
    for(auto & cm : cm_list)
    {
        if(cm.idx == cmh)
        {
            // skip
            continue;
        }
        else
        {
            for(int i = 0; i < cm.cm.size(); i++)
            {
                for(int j = 0; j < cm.cm.size(); j++)
                {
                    VertexHandle vert = cm.cm.at(i).at(j);
                    if(getVertex(vert)->point == getVertex(vh1)->point)
                    {
                        vh1_pair = vert;
                        vh1_coords_pair.x = i;
                        vh1_coords_pair.y = j;
                    }
                    else if(getVertex(vert)->point == getVertex(vh2)->point)
                    {
                        vh2_pair = vert;
                        vh2_coords_pair.x = i;
                        vh2_coords_pair.y = j;
                    }
                }
            }
            if(( vh1_pair != -1) && ( vh2_pair != -1))
            {
                cmh_pair = cm.idx;
                break;
            }
            else
            {
                // reset vh1_pair and vh2_pair
                vh1_pair = -1;
                vh2_pair = -1;
            }
        }
    }

    NeighbourNumber n = getNeighbourNumber(cmh, vh1_coords, vh2_coords);
    CMapOrientation o = getNeighbourCase(n, vh1_coords, vh2_coords, vh1_coords_pair, vh2_coords_pair);
    cm_pair.n = n;
    cm_pair.o = o;
    cm_pair.cmh_pair = cmh_pair;
    return cm_pair;
}

bool ACM::areEdgePointsEqual(const CMapIndex& e1v1,
                        const CMapIndex& e1v2,
                        const CMapIndex& e2v1,
                        const CMapIndex& e2v2)
{
    if(((e1v1 == e2v1)||(e1v2 == e2v1))
       &&
       ((e1v1 == e2v2)||(e1v2 == e2v2))
       )
    {
        return true;
    }
    else
    {
        return false;
    }
}


bool ACM::isIndexOnCorner(CMapHandle cmh, CMapIndex index)
{
    CMap* cm;
    CMapIndex location;
    cm = getCMap(cmh);
    int max = cm->cm.size()-1;
    CMapIndex v0loc(0,0); // 0,0
    CMapIndex v1loc(max,0); // max,0
    CMapIndex v2loc(max,max); // max,max
    CMapIndex v3loc(0,max); // 0,max

    // on corner
    if((v0loc == index) || (v1loc == index)
       || (v2loc == index) || (v3loc == index))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool ACM::isIndexOnBoarder(CMapHandle cmh, CMapIndex ev1, CMapIndex ev2)
{
    if(getNeighbourNumber(cmh, ev1, ev2) == -1)
    {
        return false;
    }
    else
    {
        return true;
    }
}

NeighbourNumber ACM::getNeighbourNumber(CMapHandle cmh,
                                        const CMapIndex& v1,
                                        const CMapIndex& v2)
{
    CMap* cm = getCMap(cmh);
    int max = cm->cm.size()-1;

    if( v1 == v2)
    {
        return -1;
    }
    // neighbour 0
    if(v1.y == 0 && v2.y == 0)
    {
        return 0;
    }
    // neighbour 1
    else if(v1.x == max && v2.x == max)
    {
        return 1;
    }
    // neighbour 2
    else if(v1.y == max && v2.y == max)
    {
        return 2;
    }
    // neighbour 3
    else if(v1.x == 0 && v2.x == 0)

    {
        return 3;
    }
    // invalid
    else
    {
        return -1;
    }
}

CMapOrientation ACM::getNeighbourCase(int neighbourNum,
                          const CMapIndex& v1,
                          const CMapIndex& v2,
                          const CMapIndex& v1_pair,
                          const CMapIndex& v2_pair)
{
    CMapOrientation orientation;
    switch (neighbourNum) {
        case 0:
            orientation = getNeighbourCaseFor0(v1_pair, v2_pair);
            break;

        case 1:
            orientation = getNeighbourCaseFor1(v1_pair, v2_pair);
            break;

        case 2:
            orientation = getNeighbourCaseFor2(v1_pair, v2_pair);
            break;

        case 3:
            orientation = getNeighbourCaseFor3(v1_pair, v2_pair);
            break;

        default:
            orientation = CMapOrientation::invalid;
            break;
    }
    return orientation;
}

CMapOrientation ACM::getNeighbourCaseFor0(const CMapIndex& v1_pair,
                              const CMapIndex& v2_pair)
{
    // neighbour 0 - same i and j direction
    if(areEdgePointsEqual(v1_pair, v2_pair, CMapIndex(0,1), CMapIndex(1,1)))
    {
        return CMapOrientation::posi_posj;
    }
    // neighbour 0 - i is pos j, j is neg i
    if(areEdgePointsEqual(v1_pair, v2_pair, CMapIndex(0,0), CMapIndex(0,1)))
    {
        return CMapOrientation::posj_negi;
    }
    // neighbour 0 - i is neg i, j is neg j
    if(areEdgePointsEqual(v1_pair, v2_pair, CMapIndex(1,0), CMapIndex(0,0)))
    {
        return CMapOrientation::negi_negj;
    }
    // neighbour 0 - i is neg j, j is pos i
    if(areEdgePointsEqual(v1_pair, v2_pair, CMapIndex(1,1), CMapIndex(1,0)))
    {
        return CMapOrientation::negj_posi;
    }
    // invalid
    return CMapOrientation::invalid;
}

CMapOrientation ACM::getNeighbourCaseFor1(const CMapIndex& v1_pair,
                                          const CMapIndex& v2_pair)
{
    // neighbour 1 - same i and j direction
    if(areEdgePointsEqual(v1_pair, v2_pair, CMapIndex(0,1), CMapIndex(0,0)))
    {
        return CMapOrientation::posi_posj;
    }
    // neighbour 1 - i is pos j, j is neg i
    if(areEdgePointsEqual(v1_pair, v2_pair, CMapIndex(0,0), CMapIndex(1,0)))
    {
        return CMapOrientation::posj_negi;
    }
    // neighbour 1 - i is neg i, j is neg j
    if(areEdgePointsEqual(v1_pair, v2_pair, CMapIndex(1,0), CMapIndex(1,1)))
    {
        return CMapOrientation::negi_negj;
    }
    // neighbour 1 - i is neg j, j is pos i
    if(areEdgePointsEqual(v1_pair, v2_pair, CMapIndex(1,1), CMapIndex(0,1)))
    {
        return CMapOrientation::negj_posi;
    }
    // invalid
    return CMapOrientation::invalid;
}

CMapOrientation ACM::getNeighbourCaseFor2(const CMapIndex& v1_pair,
                                          const CMapIndex& v2_pair)
{
    // neighbour 0 - same i and j direction
    if(areEdgePointsEqual(v1_pair, v2_pair, CMapIndex(0,0), CMapIndex(1,0)))
    {
        return CMapOrientation::posi_posj;
    }
    // neighbour 0 - i is pos j, j is neg i
    if(areEdgePointsEqual(v1_pair, v2_pair, CMapIndex(1,0), CMapIndex(1,1)))
    {
        return CMapOrientation::posj_negi;
    }
    // neighbour 0 - i is neg i, j is neg j
    if(areEdgePointsEqual(v1_pair, v2_pair, CMapIndex(1,1), CMapIndex(0,1)))
    {
        return CMapOrientation::negi_negj;
    }
    // neighbour 0 - i is neg j, j is pos i
    if(areEdgePointsEqual(v1_pair, v2_pair, CMapIndex(0,1), CMapIndex(0,0)))
    {
        return CMapOrientation::negj_posi;
    }
    // invalid
    return CMapOrientation::invalid;
}

CMapOrientation ACM::getNeighbourCaseFor3(const CMapIndex& v1_pair,
                                          const CMapIndex& v2_pair)
{
    // neighbour 0 - same i and j direction
    if(areEdgePointsEqual(v1_pair, v2_pair, CMapIndex(1,0), CMapIndex(1,1)))
    {
        return CMapOrientation::posi_posj;
    }
    // neighbour 0 - i is pos j, j is neg i
    if(areEdgePointsEqual(v1_pair, v2_pair, CMapIndex(1,1), CMapIndex(0,1)))
    {
        return CMapOrientation::posj_negi;
    }
    // neighbour 0 - i is neg i, j is neg j
    if(areEdgePointsEqual(v1_pair, v2_pair, CMapIndex(0,1), CMapIndex(0,0)))
    {
        return CMapOrientation::negi_negj;
    }
    // neighbour 0 - i is neg j, j is pos i
    if(areEdgePointsEqual(v1_pair, v2_pair, CMapIndex(0,0), CMapIndex(1,0)))
    {
        return CMapOrientation::negj_posi;
    }
    // invalid
    return CMapOrientation::invalid;
}

std::vector<CMapCornerNeighbour> ACM::findCMapCornerNeighbours(VertexHandle vh)
{
    // the cmap to ignore
    CMapHandle cmh_ignore = getCMapForVertex(vh);
    std::vector<CMapCornerNeighbour> cornerNeighbours;
    assert(cmh_ignore != -1);
    // look for all cms with the same vertex
    for(auto& cm : cm_list)
    {
        CMapCornerNeighbour temp;
        temp.cmh = cm.idx;
        // (0,0) || (1,1)
        if((getVertex(cm.cm.at(0).at(0))->point == getVertex(vh)->point)
            ||(getVertex(cm.cm.at(1).at(1))->point == getVertex(vh)->point)
           )
        {
            // neighbhours at 0,1 and 1,0
            temp.location = CMapIndex(0,1);
            cornerNeighbours.push_back(temp);
            temp.location = CMapIndex(1,0);
            cornerNeighbours.push_back(temp);
        }
        // (1,0)
        else if(getVertex(cm.cm.at(1).at(0))->point == getVertex(vh)->point)
        {
            // neighbhours at 0,0, 1,1 and 0,1
            temp.location = CMapIndex(0,0);
            cornerNeighbours.push_back(temp);
            temp.location = CMapIndex(1,1);
            cornerNeighbours.push_back(temp);
            temp.location = CMapIndex(0,1);
            cornerNeighbours.push_back(temp);
        }
        // (0,1)
        else if(getVertex(cm.cm.at(0).at(1))->point == getVertex(vh)->point)
        {
            // neighbhours at 0,0, 1,1 and 1,0
            temp.location = CMapIndex(0,0);
            cornerNeighbours.push_back(temp);
            temp.location = CMapIndex(1,1);
            cornerNeighbours.push_back(temp);
            temp.location = CMapIndex(1,0);
            cornerNeighbours.push_back(temp);
        }
        else
        {
        }
    }
    // remove duplicates
    std::vector<CMapCornerNeighbour>::iterator neighbour;
    for(neighbour = cornerNeighbours.begin();
         neighbour != cornerNeighbours.end();
         neighbour++)
        {
            CMap* cm1;
            VertexHandle vh1;
            Vertex* v1;
            CMapIndex v1_index = neighbour->location;
            cm1 = getCMap(neighbour->cmh);
            vh1 = cm1->cm.at(v1_index.x).at(v1_index.y);
            v1 = getVertex(vh1);

            std::vector<CMapCornerNeighbour>::iterator compare;
            // check to see if there are any equal quantities
            for(compare = neighbour+1;
                compare != cornerNeighbours.end();)
            {
                CMap* cm2;
                VertexHandle vh2;
                Vertex* v2;
                CMapIndex v2_index = compare->location;
                cm2 = getCMap(compare->cmh);
                vh2 = cm2->cm.at(v2_index.x).at(v2_index.y);
                v2 = getVertex(vh2);
                if(v1->point == v2->point)
                {
                    compare = cornerNeighbours.erase(compare);
                }
                else
                {
                    compare++;
                }
            }
        }
    return cornerNeighbours;
}

std::vector<VertexHandle> ACM::getCornerNeighbours(CMapHandle cmh, CMapIndex corner)
{
    std::vector<VertexHandle> cornerNeighbours;
    CMap* cm;
    cm = getCMap(cmh);
    int max = cm->cm.size()-1;
    CMapIndex v0loc(0,0); // 0,0
    CMapIndex v1loc(max,0); // max,0
    CMapIndex v2loc(max,max); // max,max
    CMapIndex v3loc(0,max); // 0,max

    std::vector<CMapCornerNeighbour> neighbours;
    if(v0loc == corner)
    {
        neighbours = cm->cornerVerts.at(0);
    }
    else if(v1loc == corner)
    {
        neighbours = cm->cornerVerts.at(1);

    }
    else if(v2loc == corner)
    {
        neighbours = cm->cornerVerts.at(2);

    }
    else if(v3loc == corner)
    {
        neighbours = cm->cornerVerts.at(3);
    }
    else
    {
        return std::vector<VertexHandle>();
    }

    for(const auto& neighbour : neighbours)
    {
        CMap* neighbour_cm = getCMap(neighbour.cmh);
        CMapIndex loc(neighbour.location);
        loc *= neighbour_cm->vectorScale;
        VertexHandle vert = neighbour_cm->cm.at(loc.x).at(loc.y);
        cornerNeighbours.push_back(vert);
    }

    return cornerNeighbours;
}

std::vector<VertexHandle> ACM::getBoundaryNeighbours(CMapHandle cmh,
                                                 CMapIndex edgeVert)
{
    std::vector<VertexHandle> edgeNeighbours;
    CMap* cm;
    cm = getCMap(cmh);
    int max = cm->cm.size()-1;
    CMapIndex v0loc(0,0); // 0,0
    CMapIndex v1loc(max,0); // max,0
    CMapIndex v2loc(max,max); // max,max
    CMapIndex v3loc(0,max); // 0,max

    CMapNeighbour cm_neighbour;
    CMap* cm_n_ptr;

    // make sure it is not a corner
    if((v0loc == edgeVert) || (v1loc == edgeVert)
       || (v2loc == edgeVert) || (v3loc == edgeVert))
    {
        // corner!!
        return std::vector<VertexHandle>();
    }
    // between 0,0 1,0 - bottom (0)
    else if(edgeVert.y == 0)
    {
        edgeNeighbours = getNeighboursForCase0(cmh, edgeVert);
    }
    // between 1,0 1,1 - right (1)
    else if(edgeVert.x == max)
    {
        edgeNeighbours = getNeighboursForCase1(cmh, edgeVert);
    }
    // between 0,1 1,1 - top (2)
    else if(edgeVert.y == max)
    {
        edgeNeighbours = getNeighboursForCase2(cmh, edgeVert);
    }
    // between 0,0 0,1 - left (3)
    else if(edgeVert.x == 0)
    {
        edgeNeighbours = getNeighboursForCase3(cmh, edgeVert);
    }
    else
    {
        return std::vector<VertexHandle>();
    }

    return edgeNeighbours;
}

std::vector<VertexHandle> ACM::getInternalNeighbours(CMapHandle cmh,
                                                      CMapIndex intVert)
{
    std::vector<VertexHandle> internalNeighbours;
    CMap* cm;
    cm = getCMap(cmh);
    VertexHandle vert;
    // add internal verts
    // i,0
    vert = cm->cm.at(intVert.x+1).at(intVert.y);
    internalNeighbours.push_back(vert);
    // 0,j
    vert = cm->cm.at(intVert.x).at(intVert.y+1);
    internalNeighbours.push_back(vert);
    // -i,j
    vert = cm->cm.at(intVert.x-1).at(intVert.y+1);
    internalNeighbours.push_back(vert);
    // -i,0
    vert = cm->cm.at(intVert.x-1).at(intVert.y);
    internalNeighbours.push_back(vert);
    // 0,-j
    vert = cm->cm.at(intVert.x).at(intVert.y-1);
    internalNeighbours.push_back(vert);
    // i,-j
    vert = cm->cm.at(intVert.x+1).at(intVert.y-1);
    internalNeighbours.push_back(vert);
    return internalNeighbours;
}

std::vector<VertexHandle> ACM::getNeighboursForCase0(CMapHandle cmh,
                                                     CMapIndex edgeVert)
{
    std::vector<VertexHandle> edgeNeighbours;
    CMap* cm;
    cm = getCMap(cmh);
    int max = cm->cm.size()-1;
    CMapNeighbour cm_neighbour;
    CMap* cm_n_ptr;
    VertexHandle vert;
    // add internal verts
    // i,0
    vert = cm->cm.at(edgeVert.x+1).at(edgeVert.y);
    edgeNeighbours.push_back(vert);
    // 0,j
    vert = cm->cm.at(edgeVert.x).at(edgeVert.y+1);
    edgeNeighbours.push_back(vert);
    // -i,j
    vert = cm->cm.at(edgeVert.x-1).at(edgeVert.y+1);
    edgeNeighbours.push_back(vert);
    // -i,0
    vert = cm->cm.at(edgeVert.x-1).at(edgeVert.y);
    edgeNeighbours.push_back(vert);

    // missing(0,-j):
    cm_neighbour = cm->boundaryCMs.at(0);
    cm_n_ptr = getCMap(cm_neighbour.cmh_pair);
    CMapIndex location, idx;
    CMapVector dir;
    switch (cm_neighbour.o) {
        case CMapOrientation::posi_posj:
            // edgeVert is at (x,max)
            location = CMapIndex(edgeVert.x, max);
            // 0,-j is now 0,-j
            dir = CMapVector(0,-1);
            idx.x = location.x+dir.x;
            idx.y = location.y+dir.y;
            vert = cm_n_ptr->cm.at(idx.x).at(idx.y);
            edgeNeighbours.push_back(vert);
            break;

        case CMapOrientation::posj_negi:
            // edgeVert is at (0,x)
            location = CMapIndex(0, edgeVert.x);
            // 0,-j is now i,0
            dir = CMapVector(1,0);
            idx.x = location.x+dir.x;
            idx.y = location.y+dir.y;
            vert = cm_n_ptr->cm.at(idx.x).at(idx.y);
            edgeNeighbours.push_back(vert);
            break;

        case CMapOrientation::negi_negj:
            // edgeVert is at (max-x,0)
            location = CMapIndex(max-edgeVert.x,0);
            // 0,-j is now 0,j
            dir = CMapVector(0,1);
            idx.x = location.x+dir.x;
            idx.y = location.y+dir.y;
            vert = cm_n_ptr->cm.at(idx.x).at(idx.y);
            edgeNeighbours.push_back(vert);
            break;

        case CMapOrientation::negj_posi:
            // edgeVert is at (max,max-x)
            location = CMapIndex(max,max-edgeVert.x);
            // 0,-j is now -i,0
            dir = CMapVector(-1,0);
            idx.x = location.x+dir.x;
            idx.y = location.y+dir.y;
            vert = cm_n_ptr->cm.at(idx.x).at(idx.y);
            edgeNeighbours.push_back(vert);
            break;

        default:
            break;
    }
    return edgeNeighbours;
}

std::vector<VertexHandle> ACM::getNeighboursForCase1(CMapHandle cmh,
                                                     CMapIndex edgeVert)
{
    std::vector<VertexHandle> edgeNeighbours;
    CMap* cm;
    cm = getCMap(cmh);
    int max = cm->cm.size()-1;
    CMapNeighbour cm_neighbour;
    CMap* cm_n_ptr;
    VertexHandle vert;

    // add internal verts
    // 0,j
    vert = cm->cm.at(edgeVert.x).at(edgeVert.y+1);
    edgeNeighbours.push_back(vert);
    // -i,j
    vert = cm->cm.at(edgeVert.x-1).at(edgeVert.y+1);
    edgeNeighbours.push_back(vert);
    // -i,0
    vert = cm->cm.at(edgeVert.x-1).at(edgeVert.y);
    edgeNeighbours.push_back(vert);
    // 0,-j
    vert = cm->cm.at(edgeVert.x).at(edgeVert.y-1);
    edgeNeighbours.push_back(vert);

    // missing (+i,0) (+i,-j):
    cm_neighbour = cm->boundaryCMs.at(1);
    cm_n_ptr = getCMap(cm_neighbour.cmh_pair);
    CMapIndex location, idx;
    CMapVector dir;
    switch (cm_neighbour.o) {
        case CMapOrientation::posi_posj:
            // edgeVert is at (0,y)
            location = CMapIndex(0, edgeVert.y);

            // +i,0 is now +i,0
            dir = CMapVector(1,0);
            idx.x = location.x+dir.x;
            idx.y = location.y+dir.y;
            vert = cm_n_ptr->cm.at(idx.x).at(idx.y);
            edgeNeighbours.push_back(vert);

            // +i,-j is now +i,-j
            dir = CMapVector(1,-1);
            idx.x = location.x+dir.x;
            idx.y = location.y+dir.y;
            vert = cm_n_ptr->cm.at(idx.x).at(idx.y);
            edgeNeighbours.push_back(vert);
            break;

        case CMapOrientation::posj_negi:
            // edgeVert is at (max-y,0)
            location = CMapIndex(max-edgeVert.y, 0);

            // +i,0 is now 0,+j
            dir = CMapVector(0,1);
            idx.x = location.x+dir.x;
            idx.y = location.y+dir.y;
            vert = cm_n_ptr->cm.at(idx.x).at(idx.y);
            edgeNeighbours.push_back(vert);

            // +i,-j is now +i,+j
            dir = CMapVector(1,1);
            idx.x = location.x+dir.x;
            idx.y = location.y+dir.y;
            vert = cm_n_ptr->cm.at(idx.x).at(idx.y);
            edgeNeighbours.push_back(vert);
            break;

        case CMapOrientation::negi_negj:
            // edgeVert is at (max,max-y)
            location = CMapIndex(max,max-edgeVert.y);

            // +i,0 is now -i,0
            dir = CMapVector(-1,0);
            idx.x = location.x+dir.x;
            idx.y = location.y+dir.y;
            vert = cm_n_ptr->cm.at(idx.x).at(idx.y);
            edgeNeighbours.push_back(vert);

            // +i,-j is now -i,+j
            dir = CMapVector(-1,1);
            idx.x = location.x+dir.x;
            idx.y = location.y+dir.y;
            vert = cm_n_ptr->cm.at(idx.x).at(idx.y);
            edgeNeighbours.push_back(vert);
            break;

        case CMapOrientation::negj_posi:
            // edgeVert is at (y,max)
            location = CMapIndex(edgeVert.y, max);

            // +i,0 is now 0,-j
            dir = CMapVector(0,-1);
            idx.x = location.x+dir.x;
            idx.y = location.y+dir.y;
            vert = cm_n_ptr->cm.at(idx.x).at(idx.y);
            edgeNeighbours.push_back(vert);

            // +i,-j is now -i,-j
            dir = CMapVector(-1,-1);
            idx.x = location.x+dir.x;
            idx.y = location.y+dir.y;
            vert = cm_n_ptr->cm.at(idx.x).at(idx.y);
            edgeNeighbours.push_back(vert);
            break;

        default:
            break;
    }
    return edgeNeighbours;
}


std::vector<VertexHandle> ACM::getNeighboursForCase2(CMapHandle cmh,
                                                     CMapIndex edgeVert)
{
    std::vector<VertexHandle> edgeNeighbours;
    CMap* cm;
    cm = getCMap(cmh);
    int max = cm->cm.size()-1;
    CMapNeighbour cm_neighbour;
    CMap* cm_n_ptr;
    VertexHandle vert;

    // add internal verts
    // i,0
    vert = cm->cm.at(edgeVert.x+1).at(edgeVert.y);
    edgeNeighbours.push_back(vert);
    // -i,0
    vert = cm->cm.at(edgeVert.x-1).at(edgeVert.y);
    edgeNeighbours.push_back(vert);
    // 0,-j
    vert = cm->cm.at(edgeVert.x).at(edgeVert.y-1);
    edgeNeighbours.push_back(vert);
    // i,-j
    vert = cm->cm.at(edgeVert.x+1).at(edgeVert.y-1);
    edgeNeighbours.push_back(vert);

    // missing (0,j) (-i,j):
    cm_neighbour = cm->boundaryCMs.at(2);
    cm_n_ptr = getCMap(cm_neighbour.cmh_pair);
    CMapIndex location, idx;
    CMapVector dir;
    switch (cm_neighbour.o) {
        case CMapOrientation::posi_posj:
            // edgeVert is at (x,0)
            location = CMapIndex(edgeVert.x, 0);

            // 0,j is now 0,j
            dir = CMapVector(0,1);
            idx.x = location.x+dir.x;
            idx.y = location.y+dir.y;
            vert = cm_n_ptr->cm.at(idx.x).at(idx.y);
            edgeNeighbours.push_back(vert);

            // -i,j is now -i,j
            dir = CMapVector(-1,1);
            idx.x = location.x+dir.x;
            idx.y = location.y+dir.y;
            vert = cm_n_ptr->cm.at(idx.x).at(idx.y);
            edgeNeighbours.push_back(vert);
            break;

        case CMapOrientation::posj_negi:
            // edgeVert is at (max, x)
            location = CMapIndex(max, edgeVert.x);

            // 0,j is now -i,0
            dir = CMapVector(-1,0);
            idx.x = location.x+dir.x;
            idx.y = location.y+dir.y;
            vert = cm_n_ptr->cm.at(idx.x).at(idx.y);
            edgeNeighbours.push_back(vert);

            // -i,j is now -i,-j
            dir = CMapVector(-1,-1);
            idx.x = location.x+dir.x;
            idx.y = location.y+dir.y;
            vert = cm_n_ptr->cm.at(idx.x).at(idx.y);
            edgeNeighbours.push_back(vert);
            break;

        case CMapOrientation::negi_negj:
            // edgeVert is at (max-x, max)
            location = CMapIndex(max-edgeVert.x, max);

            // 0,j is now 0,-j
            dir = CMapVector(0,-1);
            idx.x = location.x+dir.x;
            idx.y = location.y+dir.y;
            vert = cm_n_ptr->cm.at(idx.x).at(idx.y);
            edgeNeighbours.push_back(vert);

            // -i,j is now i,-j
            dir = CMapVector(1,-1);
            idx.x = location.x+dir.x;
            idx.y = location.y+dir.y;
            vert = cm_n_ptr->cm.at(idx.x).at(idx.y);
            edgeNeighbours.push_back(vert);
            break;

        case CMapOrientation::negj_posi:
            // edgeVert is at (0,max-x)
            location = CMapIndex(0, max-edgeVert.x);

            // 0,j is now i,0
            dir = CMapVector(1,0);
            idx.x = location.x+dir.x;
            idx.y = location.y+dir.y;
            vert = cm_n_ptr->cm.at(idx.x).at(idx.y);
            edgeNeighbours.push_back(vert);

            // -i,j is now i,j
            dir = CMapVector(1,1);
            idx.x = location.x+dir.x;
            idx.y = location.y+dir.y;
            vert = cm_n_ptr->cm.at(idx.x).at(idx.y);
            edgeNeighbours.push_back(vert);
            break;
            
        default:
            break;
    }
    return edgeNeighbours;
}


std::vector<VertexHandle> ACM::getNeighboursForCase3(CMapHandle cmh,
                                                     CMapIndex edgeVert)
{
    std::vector<VertexHandle> edgeNeighbours;
    CMap* cm;
    cm = getCMap(cmh);
    int max = cm->cm.size()-1;
    CMapNeighbour cm_neighbour;
    CMap* cm_n_ptr;
    VertexHandle vert;

    // add internal verts
    // i,0
    vert = cm->cm.at(edgeVert.x+1).at(edgeVert.y);
    edgeNeighbours.push_back(vert);
    // 0,j
    vert = cm->cm.at(edgeVert.x).at(edgeVert.y+1);
    edgeNeighbours.push_back(vert);
    // 0,-j
    vert = cm->cm.at(edgeVert.x).at(edgeVert.y-1);
    edgeNeighbours.push_back(vert);
    // i,-j
    vert = cm->cm.at(edgeVert.x+1).at(edgeVert.y-1);
    edgeNeighbours.push_back(vert);

    // missing (-i,j) (-i,0):
    cm_neighbour = cm->boundaryCMs.at(3);
    cm_n_ptr = getCMap(cm_neighbour.cmh_pair);
    CMapIndex location, idx;
    CMapVector dir;
    switch (cm_neighbour.o) {
        case CMapOrientation::posi_posj:
            // edgeVert is at (max,y)
            location = CMapIndex(max, edgeVert.y);

            // -i,j is now -i,j
            dir = CMapVector(-1,1);
            idx.x = location.x+dir.x;
            idx.y = location.y+dir.y;
            vert = cm_n_ptr->cm.at(idx.x).at(idx.y);
            edgeNeighbours.push_back(vert);

            // -i,0 is now -i,0
            dir = CMapVector(-1,0);
            idx.x = location.x+dir.x;
            idx.y = location.y+dir.y;
            vert = cm_n_ptr->cm.at(idx.x).at(idx.y);
            edgeNeighbours.push_back(vert);
            break;

        case CMapOrientation::posj_negi:
            // edgeVert is at (max-y,max)
            location = CMapIndex(max-edgeVert.y, max);

            // -i,j is now -i,-j
            dir = CMapVector(-1,-1);
            idx.x = location.x+dir.x;
            idx.y = location.y+dir.y;
            vert = cm_n_ptr->cm.at(idx.x).at(idx.y);
            edgeNeighbours.push_back(vert);

            // -i,0 is now 0,-j
            dir = CMapVector(0,-1);
            idx.x = location.x+dir.x;
            idx.y = location.y+dir.y;
            vert = cm_n_ptr->cm.at(idx.x).at(idx.y);
            edgeNeighbours.push_back(vert);
            break;

        case CMapOrientation::negi_negj:
            // edgeVert is at (0,max-y)
            location = CMapIndex(max, edgeVert.y);

            // -i,j is now i,-j
            dir = CMapVector(1,-1);
            idx.x = location.x+dir.x;
            idx.y = location.y+dir.y;
            vert = cm_n_ptr->cm.at(idx.x).at(idx.y);
            edgeNeighbours.push_back(vert);

            // -i,0 is now i,0
            dir = CMapVector(1,0);
            idx.x = location.x+dir.x;
            idx.y = location.y+dir.y;
            vert = cm_n_ptr->cm.at(idx.x).at(idx.y);
            edgeNeighbours.push_back(vert);
            break;

        case CMapOrientation::negj_posi:
            // edgeVert is at (y,0)
            location = CMapIndex(edgeVert.y, 0);

            // -i,j is now i,j
            dir = CMapVector(1,1);
            idx.x = location.x+dir.x;
            idx.y = location.y+dir.y;
            vert = cm_n_ptr->cm.at(idx.x).at(idx.y);
            edgeNeighbours.push_back(vert);

            // -i,0 is now 0,j
            dir = CMapVector(0,1);
            idx.x = location.x+dir.x;
            idx.y = location.y+dir.y;
            vert = cm_n_ptr->cm.at(idx.x).at(idx.y);
            edgeNeighbours.push_back(vert);
            break;
            
        default:
            break;
    }
    return edgeNeighbours;
}

EdgeDirection ACM::getEdgeDirection(CMapIndex v1, CMapIndex v2)
{
    // vertical
    if(v1.x == v2.x)
    {
        return EdgeDirection::vert;
    }
    // horizontal
    else if(v1.y == v2.y)
    {
        return EdgeDirection::horz;
    }
    // diagonal
    else
    {
        return EdgeDirection::diag;
    }
}
