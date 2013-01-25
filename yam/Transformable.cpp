/*  
    � Copyright 2012, David Welter
    
    This file is part of PEST++.
   
    PEST++ is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    PEST++ is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with PEST++.  If not, see<http://www.gnu.org/licenses/>.
*/
#include "network_wrapper.h"
#include <ostream>
#include <string>
#include <sstream>
#include <vector>
#include <fstream>
#include <iomanip>
#include <utility>
#include <cassert>
#include <memory>
#include <lapackpp.h>
#include "Transformable.h"
#include "pest_error.h"
#include "utilities.h"

using std::string;
using std::map;
using std::ostream;
using std::endl;
using namespace pest_utils;

const double Transformable::no_data = -9.99E99;

Transformable::Transformable(const Transformable &copyin) : items(copyin.items)
{
}

Transformable::Transformable(const Transformable &copyin, const vector<string> &copy_names)
{
	for (vector<string>::const_iterator b=copy_names.begin(), e=copy_names.end(); b!=e; ++b) {
		items[*b] = copyin.get_rec(*b);
	}
}


const Transformable& Transformable::operator=(const Transformable &rhs)
{
	items = rhs.items;
	return *this;
}

Transformable& Transformable::operator+=(const Transformable &rhs)
{
	for(Transformable::iterator b=items.begin(), e=items.end(); b!=e; ++b)
	{
		assert(rhs.items.find(b->first) != rhs.items.end());
		b->second += rhs.items.find(b->first)->second;
	}
    return *this;
 }


Transformable& Transformable::operator-=(const Transformable &rhs)
{
	for(Transformable::iterator b=items.begin(), e=items.end(); b!=e; ++b)
	{
		assert(rhs.items.find(b->first) != rhs.items.end());
		b->second -= rhs.items.find(b->first)->second;
	}
    return *this;
 }

Transformable Transformable::operator-(const Transformable &rhs) const
{
	Transformable ret_val(*this);
	ret_val -= rhs;
	return ret_val;

	
}

double &Transformable::operator[](const string &name)
{
	return items[name];
}

pair<Transformable::iterator,bool> Transformable::insert(const string &name, double value)
{
	pair<string, double> rec(name, value);
	return items.insert(rec);
}

pair<Transformable::iterator,bool>  Transformable::insert(const pair<string, double> &x)
{
	return items.insert(x);
}

void Transformable::insert(const vector<string> &name_vec, const vector<double> &value_vec)
{
    assert(name_vec.size() == value_vec.size());
    int vec_size = name_vec.size();
    items.reserve(vec_size);
	for(int i=0; i<vec_size; ++i)
	{
        insert(pair<string, double>(name_vec[i], value_vec[i]));
	}
}

void Transformable::insert(iterator first, iterator last )
{
	items.insert(first, last);
}

size_t Transformable::erase(const string &name)
{
	return items.erase(name);
}

Transformable::iterator Transformable::find(const string &name)
{
	return items.find(name);
}

Transformable::const_iterator Transformable::find(const string &name) const
{
	return items.find(name);
}

const double* Transformable::get_rec_ptr(const string &name) const
{
	const double *ret_val = 0;
	Transformable::const_iterator iter = find(name);
	if (iter != end()) {
		ret_val = &((*iter).second);
	}
	return ret_val;
}

const double Transformable::get_rec(const string &name) const
{
	double ret_val = no_data;
	Transformable::const_iterator iter = find(name);
	if (iter != end()) {
		ret_val = (*iter).second;
	}
	else {
		throw(Transformable_value_error(name));
	}
	return ret_val;
}

void Transformable::update_rec(const string &name, double value)
{
	Transformable::iterator iter = find(name);
	assert(iter != end());
	if (iter != end()) {
		(*iter).second = value;
	}
	else {
		throw(Transformable_value_error(name));
	}
}

void Transformable::update(const vector<string> &names, const vector<double> &values)
{
    items.clear();
    assert(names.size() == values.size());
    size_t n_rec = names.size();
    items.reserve(n_rec);
    for (size_t i=0; i<n_rec; ++i)
	{
        items.insert(make_pair(names[i], values[i]));
	}
}

vector<double> Transformable::get_data_vector(const vector<string> &keys) const
{
	vector<double> v;
	v.resize(items.size(), 0.0);

	double value;
	int i = 0;

	for(vector<string>::const_iterator b=keys.begin(), e=keys.end();
		b!=e; ++b, ++i)
	{
		value = get_rec((*b));
		v[i] = value;
	}
	return v;
}

vector<string> Transformable::get_keys() const
{
	vector<string> ret_val;
	ret_val.reserve(size());
	for(Transformable::const_iterator b=begin(), e=end();
		b!=e; ++b)
	{
		ret_val.push_back((*b).first);
	}
	return ret_val;
}

void Transformable::add_upgrade(const vector<string> &keys, const LaVectorDouble &del_values)
{
	int i = 0;
	Transformable::iterator found;
	Transformable::iterator not_found=items.end();

	for(vector<string>::const_iterator b=keys.begin(), e=keys.end(); b!=e; ++b, ++i)
	{
		found = items.find(*b);
		assert(found != not_found);
		(*found).second += del_values(i);
	}
}

ostream& operator<<(ostream& out, const Transformable &rhs)
{
	for(Transformable::const_iterator b=rhs.begin(), e=rhs.end();
		b!=e; ++b)
	{
		out << b->first << " = " << b->second << endl;
	}
	return out;
}

