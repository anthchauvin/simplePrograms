#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <iterator>
#include <vector>

using namespace std;

class ship
{
public:
	ship() = delete;

	ship(ship const&) = delete;
	ship& operator = (ship const&) = delete;

	ship(ship&& s) :		//move constructor
		name_(s.name_)
	{
		s.name_ = "";			//string version of null
	};
	ship& operator = (ship&&)	 //move a op
	{
		return *this;
	}

	virtual ~ship(){};		//virtual destructor

	explicit ship(string const& name) :
		name_(name) {};

	explicit ship(string&& name) :
		name_(move(name)) {};

	virtual void sail() = 0;

	virtual string get_ship_type() const
	{
		return "ship";
	}
	string const& get_name() const
	{
		return name_;
	}
	string get_type_name_addr() const
	{
		ostringstream buf;
		buf<<get_ship_type()<<' '<<get_name()<<" ["<<this<<']';
		return buf.str();
	}

protected:
	void do_ship_only_move(ship&& s)
	{
		name_ = move(s.name_);
	}

private:
	string name_;
};

class battle_ship : public virtual ship
{
public:
	battle_ship() = delete;

	battle_ship(battle_ship const&) = delete;
	battle_ship& operator = (battle_ship const&) = delete;

	battle_ship(battle_ship&& b) :
		ship(move(b)){}

	battle_ship& operator = (battle_ship&& b)
	{
		ship::operator = (move(b));
		do_battle_ship_only_move(move(b));
		return *this;
	}

	explicit battle_ship(string const& name) :
		ship(name){}

	explicit battle_ship(string&& name) :
		ship(name){}

	string get_ship_type() const override
	{
		return "Battle_ship";
	}

	void sail() override
	{
		cout << get_type_name_addr() << " off to war!\n";
	}

	virtual void fire_weapon()
	{
		cout << get_type_name_addr() << " firing cannon!\n";
	}

protected:

	void do_battle_ship_only_move(battle_ship&&){}
};

template <typename T>
class cargo_ship : public virtual ship		//cargo ship class def
{
public:
	cargo_ship() = delete;

	cargo_ship(cargo_ship const&) = delete;
	cargo_ship& operator = (cargo_ship const&) = delete;

	cargo_ship(cargo_ship&& c) :
		ship(move(c))
	{
		this->cargo_ = c.cargo_;
	}

	cargo_ship& operator = (cargo_ship&& c)
	{
		ship::operator = (move(c));
		do_cargo_ship_only_move(move(c));
		return *this;
	}

	explicit cargo_ship(string const& name) :
		ship(name){}

	explicit cargo_ship(string&& name) :
		ship(name){}

	template <typename Iter>
	explicit cargo_ship(string name, Iter first, Iter last) :
		ship(name), cargo_(first,last){}

	string get_ship_type() const override
	{
		return "cargo_ship";
	}

	void sail() override						//In assignment instructions, says to call it ship(), but what I believe it to be is sail()
	{
		cout << get_type_name_addr() << " setting sail!\n";
	}

	vector<T>& get_cargo()
	{
		return cargo_;
	}

	vector<T>& get_cargo() const
	{
		return cargo_;
	}

protected:

	void do_cargo_ship_only_move(cargo_ship&& s)
	{
		this->cargo_ = move(s.cargo_);
	}

	vector<T> cargo_;
};

template <class T>
class smuggler_ship : public virtual cargo_ship<T>, public virtual battle_ship		//Instances of get_type_name_addr in this class will refer to battle_ship class, or else it will be ambiguous
{
public:
	smuggler_ship() = delete;

	smuggler_ship(smuggler_ship const&) = delete;
	smuggler_ship& operator = (smuggler_ship const&) =  delete;

	smuggler_ship(smuggler_ship&& s) :
		ship(move(s)), cargo_ship<T>(move(s)), battle_ship(move(s)) {}

	smuggler_ship& operator = (smuggler_ship&& s)
	{
		do_smuggler_ship_only_move(move(s));
		return *this;
	}

	explicit smuggler_ship(string const& name) :
		ship(name), cargo_ship<T>(name), battle_ship(name){}

	explicit smuggler_ship(string&& name) :
		ship(name), cargo_ship<T>(name), battle_ship(name){}

	string get_ship_type() const override
	{
		return "smuggler_ship";
	}

	void sail() override
	{
		cout << battle_ship::get_type_name_addr() << " off to pillage!\n";
	}

	void fire_weapon() override
	{
		cout << battle_ship::get_type_name_addr() << " launching harpoon!";
	}

	void steal(ship& s)
	{
		cargo_ship<T>* cs = dynamic_cast<cargo_ship<T>*>(&s);
		smuggler_ship<T>* ss = dynamic_cast<smuggler_ship<T>*>(&s);

		if (ss != nullptr)
		{
			ostringstream buf;
			buf << battle_ship::get_type_name_addr() << " cannot steal from another " << ss->battle_ship::get_type_name_addr() << '!';

			throw runtime_error(buf.str());
		}
		else if (cs != nullptr)
		{
			cout << battle_ship::get_type_name_addr() << " is stealing half of " << cs->cargo_ship<T>::get_type_name_addr() << "!\n";
			auto middle = begin(cs->get_cargo()) + distance(begin(cs->get_cargo()), end(cs->get_cargo())) / 2;

			for (auto a = middle; a != cs->get_cargo().end(); ++a)
			{
				this->get_cargo().emplace_back(move(*a));
			}

			cs->get_cargo().erase(middle, cs->get_cargo().end());

		}
		else
		{
			ostringstream buf;
			buf << battle_ship::get_type_name_addr() << " cannot steal from a " << s.get_type_name_addr() << '!';

			throw runtime_error(buf.str());
		}
	}

protected:

	void do_smuggler_ship_only_move(smuggler_ship&& s)
	{
		do_ship_only_move(s);
		do_cargo_ship_only_move(s);
		do_battle_ship_only_move(s);
	}

};
void sail_all_ships(vector<unique_ptr<ship>>& ships)
{
	for(auto& ship : ships)
		ship->sail();
}

void fight(battle_ship& b1, battle_ship& b2)
{
	b1.fire_weapon();
	b2.fire_weapon();
}

void sail(unique_ptr<ship> const& s)
{
	s->sail();
}

template <typename ShipType, typename... Args>				//Supplied documentation
unique_ptr<ship> create_ship(Args&&... args)
{
	return unique_ptr<ship>(new ShipType(forward<Args>(args)...));
}

template <typename ShipType, typename... Args>
unique_ptr<ShipType> create_ship_as_is(Args&&... args)
{
	return unique_ptr<ShipType>(new ShipType(forward<Args>(args)...));
}

int main()
{
	vector<unique_ptr<ship>> shipyard;
	shipyard.emplace_back(create_ship<cargo_ship<int>>("Storage Master"));
	shipyard.emplace_back(create_ship<battle_ship>("HMS Awesome"));
	shipyard.emplace_back(create_ship<smuggler_ship<int>>("Bounty Hunter"));

	//vector<unique_ptr<ship>> shipyard_new_location(move(shipyard));

	//vector<unique_ptr<ship>> wont_work2(shipyard);

	/*vector<smuggler_ship<int>> wont_work2{
		smuggler_ship<int>("Thief 1 Ship"),
		smuggler_ship<int>("Thief 2 Ship")
	};*/

	cout << "Sail all ships...\n";
	sail_all_ships(shipyard);

	cout << "\nFight between b1 and b2...\n";
	battle_ship b1("HMCS Hunter");
	battle_ship b2("SS Teacup");
	fight(b1, b2);

	cout << "\nFight b1 with last ship in shipyard...\n";
	fight(
		b1,
		dynamic_cast<smuggler_ship<int>&>(*shipyard.back())
	);

	cout << "\nSail one-by-one...\n";
	for(auto const& i : shipyard)
		sail(i);

	cout << "\nSteal cargo...\n";
	auto ss = create_ship_as_is<smuggler_ship<int>>("Pirate Ship");
	auto cs = create_ship<cargo_ship<int>>("Cargo Ship");
	auto bs = create_ship<battle_ship>("Non-Cargo Ship");
	ss->steal(*cs);
	try { ss->steal(*bs); } catch (exception& e) { cout << e.what() << '\n'; }
}

