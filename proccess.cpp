#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <cmath>
#include <set>
#include <list>
#include <algorithm>

#include <unistd.h>

struct point
{
	std::string name;
	int x, y;
};

int distance(point a, point b)
{
	auto out = sqrt( pow( abs(a.x - b.x) , 2) + pow( abs( a.y - b.y), 2));
	return out * 10;
}

struct Route
{
	std::set<std::string> rSet;
	std::list<point> rList;
	int totalDistance = 0;
	int maxDistance = 4500;
	int minStep = 800;
	int maxStep = 1000;

	std::string startP (void) const
	{
		if(rList.size()) return rList.front().name;
		return "";
	}

	bool valid (point p)
	{
		if(rSet.count(p.name)) return false;
		if(distance(p, rList.back()) > maxStep) return false; 
		if(distance(p, rList.back()) < minStep) return false; 
		if(totalDistance + distance(p, rList.back()) > maxDistance) return false;


		return true;
	}

	void add (point p)
	{
		if(rList.size()) totalDistance += distance(p, rList.back());
		rSet.insert(p.name);
		rList.push_back(p);
	}

	void pop (void)
	{
		point temp = rList.back();
		rList.pop_back();
		rSet.erase(temp.name);

		if(rList.size())
		{
			totalDistance -= distance(rList.back(), temp);
		}
	}
};


std::vector<point> startPoints;
std::vector<point> pointCloud;
std::vector<point> endPoints;

auto eatPointsFile (std::ifstream& in, std::vector<point>& res)
{
	std::string line;
	std::stringstream stream;
	while (std::getline( in, line))
	{
		stream = std::stringstream(line);
		int x, y;
		std::string name;
		std::string gz;

		stream >> name >> gz >> x >> y;
		res.push_back({ name, x, y});
	}
}

std::vector<Route> validRoutes;
std::vector<Route> nightRoutes;


void routeHelper (Route& route, std::vector<point>& points, std::vector<Route>& out)
{
	if(route.rList.size() >= 5)
	{
		if(route.valid( endPoints[0]))
		{
			route.add(endPoints[0]);
			out.push_back(route);
			route.pop();
		}
		return;
	}

	for(const auto& p : points)
	{
		if(route.valid(p))
		{
			route.add(p);
			routeHelper(route, points, out);
			route.pop();
		}
	}
}

void findValidRoute (void)
{
	Route route;
	for(const auto& p : startPoints)
	{
		route.add(p);
		routeHelper(route, pointCloud, validRoutes);
		route.pop();
	}

	Route nightRoute;
	nightRoute.maxDistance = 3500;
	nightRoute.minStep = 600;
	nightRoute.maxStep = 800;
	for(const auto& p : startPoints)
	{
		nightRoute.add(p);
		routeHelper(nightRoute, pointCloud, nightRoutes);
		nightRoute.pop();
	}
}


auto main (int argc, char** argv) -> int
{	

	std::ifstream pointsF, startPointsF, endPointsF;
	int opt;

	while ((opt = getopt(argc, argv, "p:e:s:")) != -1)
	{
		switch (opt)
		{
			case 'p':
				pointsF = std::ifstream(optarg);
				break;
			case 'e':
				endPointsF = std::ifstream(optarg);
				break;
			case 's':
				startPointsF = std::ifstream(optarg);
				break;
			default: 
				std::cerr << "Usage: " << argv[0] << " [-p point file] [-e endPoints] [-s startPoints]\n";
				exit(EXIT_FAILURE);
			}
	}

	eatPointsFile(pointsF, pointCloud);
	eatPointsFile(endPointsF, endPoints);
	eatPointsFile(startPointsF, startPoints);
	

	findValidRoute();
	std::cout << "total day routes found: " << validRoutes.size() << std::endl;
	std::cout << "total night routes found: " << nightRoutes.size() << std::endl;

	std::sort(validRoutes.begin(), validRoutes.end(), [](const auto &l, const auto &r){if(l.startP() != r.startP()) return l.startP() < r.startP(); return  l.totalDistance > r.totalDistance;});
	std::sort(nightRoutes.begin(), nightRoutes.end(), [](const auto &l, const auto &r){if(l.startP() != r.startP()) return l.startP() < r.startP(); return  l.totalDistance > r.totalDistance;});

	std::cout << "day routes: " << std::endl;

	for(const auto& R: validRoutes)
	{
		for(const auto& p : R.rList) std::cout << p.name << ' ';
		std::cout << std::endl;
		auto last = R.rList.front();
		for(const auto& p : R.rList) 
		{
			std::cout << distance(last, p) << ' ';
			last = p;
		}
		std::cout << "total distance: " << R.totalDistance;
		std::cout << std::endl;
		for(const auto& p : R.rList) 
		{
			std::cout << "TH " << p.x << ' ' << p.y << '	';
		}
		std::cout << std::endl << std::endl;

	}
	
	std::cout << "night routes ========== " << std::endl;

	for(const auto& R: nightRoutes)
	{
		for(const auto& p : R.rList) std::cout << p.name << ' ';
		std::cout << std::endl;
		auto last = R.rList.front();
		for(const auto& p : R.rList) 
		{
			std::cout << distance(last, p) << ' ';
			last = p;
		}
		std::cout << "total distance: " << R.totalDistance;
		std::cout << std::endl;
		for(const auto& p : R.rList) 
		{
			std::cout << "TH " << p.x << ' ' << p.y << '	';
		}
		std::cout << std::endl << std::endl;

	}




}


