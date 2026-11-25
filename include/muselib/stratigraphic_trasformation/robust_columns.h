#ifndef MUSE_ROBUST_COLUMNS_H
#define MUSE_ROBUST_COLUMNS_H
#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <map>
#include <numeric>
#include <stdexcept>
#include <vector>

namespace MUSE {
// Full-line intersections with the final tetrahedral boundary. The BVH only
// rejects disjoint projected boxes; long-double barycentrics decide inclusion.
class RobustColumns {
    struct Triangle {
        std::array<std::array<long double,3>,3> p;
        std::array<double,2> lo, hi;
        int crossing;
    };
    struct Node {
        std::array<double,2> lo, hi;
        size_t begin, end;
        int left=-1, right=-1;
    };
    std::vector<Triangle> triangles;
    std::vector<size_t> order;
    std::vector<Node> nodes;
    std::array<double,3> origin;
    int axis, u, v;
    int build(size_t begin, size_t end) {
        Node n;
        n.begin=begin; n.end=end;
        n.lo.fill(std::numeric_limits<double>::infinity());
        n.hi.fill(-std::numeric_limits<double>::infinity());
        for(size_t i=begin;i<end;++i) for(int d=0;d<2;++d) {
            n.lo[d]=std::min(n.lo[d],triangles[order[i]].lo[d]);
            n.hi[d]=std::max(n.hi[d],triangles[order[i]].hi[d]);
        }
        int id=static_cast<int>(nodes.size()); nodes.push_back(n);
        if(end-begin>16) {
            int d=(n.hi[1]-n.lo[1]>n.hi[0]-n.lo[0]);
            size_t mid=begin+(end-begin)/2;
            std::nth_element(order.begin()+begin,order.begin()+mid,order.begin()+end,
                [&](size_t a,size_t b){return triangles[a].lo[d]+triangles[a].hi[d]<triangles[b].lo[d]+triangles[b].hi[d];});
            int left=build(begin,mid), right=build(mid,end);
            nodes[id].left=left; nodes[id].right=right;
        }
        return id;
    }
    void hits(int id, long double x, long double y, std::vector<std::pair<double,int>>& out) const {
        const auto &n=nodes[id];
        if(x<n.lo[0]-tol || x>n.hi[0]+tol || y<n.lo[1]-tol || y>n.hi[1]+tol) return;
        if(n.left>=0) {hits(n.left,x,y,out);hits(n.right,x,y,out);return;}
        for(size_t i=n.begin;i<n.end;++i) {
            const auto &t=triangles[order[i]];
            const auto &p=t.p;
            long double ax=p[1][u]-p[0][u], ay=p[1][v]-p[0][v];
            long double bx=p[2][u]-p[0][u], by=p[2][v]-p[0][v];
            long double det=ax*by-ay*bx, dx=x-p[0][u], dy=y-p[0][v];
            long double a=(dx*by-dy*bx)/det, b=(ax*dy-ay*dx)/det, c=1-a-b;
            if(a < -tol*std::hypot(bx,by)/std::abs(det) ||
               b < -tol*std::hypot(ax,ay)/std::abs(det) ||
               c < -tol*std::hypot(bx-ax,by-ay)/std::abs(det)) continue;
            a=std::max(0.L,a); b=std::max(0.L,b); c=std::max(0.L,c);
            double z=static_cast<double>((c*p[0][axis]+a*p[1][axis]+b*p[2][axis])/(a+b+c));
            out.emplace_back(z,t.crossing);
        }
    }
public:
    double tol;
    enum Status {ok, missing, tangent, ambiguous};
    struct Bounds {double bottom=0, top=0; Status status=missing;};
    template<class Mesh> RobustColumns(const Mesh &mesh,int direction,double geometric_tolerance=0):axis(direction) {
        if(axis<0 || axis>2 || !mesh.num_verts() || !mesh.num_polys()) throw std::runtime_error("Invalid column mesh/direction");
        u=axis==0?1:0; v=axis==2?1:2;
        double extent=0;
        for(int d=0;d<3;++d) {
            double lo=mesh.vert(0)[d],hi=lo;
            for(unsigned i=0;i<mesh.num_verts();++i) {
                double x=mesh.vert(i)[d];
                if(!std::isfinite(x)) throw std::runtime_error("Nonfinite mesh coordinates");
                lo=std::min(lo,x);hi=std::max(hi,x);
            }
            origin[d]=lo+(hi-lo)/2;extent=std::max(extent,hi-lo);
        }
        // Model-space resolution, independent of machine epsilon and coordinate offset.
        if(!std::isfinite(geometric_tolerance) || geometric_tolerance<0)
            throw std::runtime_error("Geometric tolerance must be finite and nonnegative");
        tol=geometric_tolerance>0 ? geometric_tolerance : 1e-9*extent;
        if(!std::isfinite(tol) || tol<=0) throw std::runtime_error("Invalid model extent/tolerance");
        for(unsigned pid=0;pid<mesh.num_polys();++pid)
            if(mesh.poly_verts_id(pid).size()!=4) throw std::runtime_error("Robust columns require linear tetrahedra");
        std::map<std::array<unsigned,2>,unsigned> edges;
        for(unsigned fid=0;fid<mesh.num_faces();++fid) {
            const auto &incident=mesh.adj_f2p(fid);
            if(incident.size()>2) throw std::runtime_error("Nonmanifold tetrahedral face");
            if(incident.size()!=1) continue;
            auto ids=mesh.face_verts_id(fid);
            if(ids.size()!=3) throw std::runtime_error("Nontriangular tetrahedral boundary");
            for(int j=0;j<3;++j) {
                std::array<unsigned,2> edge{{ids[j],ids[(j+1)%3]}};
                std::sort(edge.begin(),edge.end());++edges[edge];
            }
            unsigned opposite=std::numeric_limits<unsigned>::max();
            for(auto vid:mesh.poly_verts_id(incident[0]))
                if(std::find(ids.begin(),ids.end(),vid)==ids.end()) opposite=vid;
            if(opposite==std::numeric_limits<unsigned>::max()) throw std::runtime_error("Missing opposite tetrahedral vertex");
            Triangle t;
            for(int j=0;j<3;++j) for(int d=0;d<3;++d) t.p[j][d]=mesh.vert(ids[j])[d]-origin[d];
            std::array<long double,3> a,b,normal;
            for(int d=0;d<3;++d) {a[d]=t.p[1][d]-t.p[0][d];b[d]=t.p[2][d]-t.p[0][d];}
            normal={{a[1]*b[2]-a[2]*b[1],a[2]*b[0]-a[0]*b[2],a[0]*b[1]-a[1]*b[0]}};
            long double inward=0;
            for(int d=0;d<3;++d) inward+=normal[d]*(mesh.vert(opposite)[d]-origin[d]-t.p[0][d]);
            if(inward==0) throw std::runtime_error("Degenerate boundary tetrahedron");
            if(normal[axis]==0) continue;
            t.crossing=((normal[axis]>0)?1:-1)*((inward>0)?-1:1);
            for(int d=0;d<2;++d) {
                int component=d==0?u:v;
                t.lo[d]=static_cast<double>(std::min({t.p[0][component],t.p[1][component],t.p[2][component]}));
                t.hi[d]=static_cast<double>(std::max({t.p[0][component],t.p[1][component],t.p[2][component]}));
            }
            triangles.push_back(t);
        }
        for(const auto &edge:edges) if(edge.second!=2) throw std::runtime_error("Boundary is not a closed two-manifold");
        if(triangles.empty()) throw std::runtime_error("No transverse boundary faces");
        order.resize(triangles.size());std::iota(order.begin(),order.end(),0);build(0,order.size());
    }
    template<class Point> Bounds bounds(const Point &point) const {
        for(int d=0;d<3;++d) if(!std::isfinite(point[d])) throw std::runtime_error("Nonfinite query coordinates");
        std::vector<std::pair<double,int>> h;
        hits(0,point[u]-origin[u],point[v]-origin[v],h);
        std::sort(h.begin(),h.end());
        std::vector<double> z;
        std::vector<int> signs;
        double previous=0;
        for(const auto &hit:h) {
            if(z.empty() || hit.first-previous>tol) {z.push_back(hit.first);signs.push_back(0);}
            signs.back() |= hit.second<0?1:2;
            previous=hit.first;
        }
        Bounds result;
        if(z.size()==2 && signs[0]==1 && signs[1]==2 && z[1]-z[0]>tol)
            {result.bottom=z[0]+origin[axis];result.top=z[1]+origin[axis];result.status=ok;}
        else if(z.size()==1) result.status=tangent;
        else if(!z.empty()) result.status=ambiguous;
        return result;
    }
};
}
#endif
