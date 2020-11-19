

#define _CRT_SECURE_NO_WARNINGS

//#include <vld.h>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>



// ClauText
#define ARRAY_QUEUE std::deque // chk?
#define VECTOR std::vector
#include <wiz/ClauText.h>
// ~ClauText

#define ENTER '\n' // cf) std::endl

namespace Lint {
	wiz::StringBuilder builder(102400);
	bool cout_off = false; // print yes or no?
	wiz::Option opt;

	// justOne + optional_ -> no ok?
	// onemore + optional_ -> no ok?
	class Option
	{
	public:
		enum class Type_ { ANY, INT, FLOAT, QUOTED_STRING, STRING, DATETIME, DATETIME_A, DATETIME_B };

		enum class Id_ { NONE, ID, TOTAL_ID };
		enum class OneMore_ { NONE, ONEMORE, JUSTONE };
		enum class Required_ { REQUIRED, OPTIONAL_ };
		enum class EmptyUT_ { NONE, OFF, ON };

		enum class Order_ { NONE, OFF, ON }; // with global??
		enum class Multiple_ { NONE, OFF, ON };
	public:
		std::vector<Type_> type;
		Id_ id;
		OneMore_ onemore;
		EmptyUT_ empty_ut;
		Required_ required;
		std::string event_ids;

		std::string prefix;
	public:
		Option() : type(), //id(Id_::NONE), 
			//onemore(OneMore_::NONE), 
			required(Required_::REQUIRED),
			empty_ut(EmptyUT_::NONE)
		{
			//
		}
	public:
		Option& Type(Type_ type) {
			this->type.push_back(type);
			return *this;
		}
		Option& Id(Id_ id) {
			this->id = id;
			return *this;
		}
		Option& OneMore(OneMore_ onemore) {
			this->onemore = onemore;
			return *this;
		}
		Option& EmptyUT(EmptyUT_ empty_ut) {
			this->empty_ut = empty_ut;
			return *this;
		}
		Option& Required(Required_ required) {
			this->required = required;
			return *this;
		}
		Option& Event(const std::string& event_id) {
			this->event_ids += " ";
			this->event_ids += event_id;
			return *this;
		}
	public:
		// check valid function?
		bool IsValid() const {
			// to do
		}
	};


	Option OptionFrom(const std::string& option_str)
	{
		Option option;
		std::string::size_type start = 0;
		std::string::size_type find_percent = std::string::npos; // % : delimeter.

		if ((find_percent = option_str.find('%', start)) == std::string::npos)
		{
			option.prefix = option_str;
		}
		else {
			option.prefix = option_str.substr(start, find_percent - start);
		}

		while ((find_percent = option_str.find('%', start)) != std::string::npos)
		{
			std::string::size_type end_ = option_str.find('%', find_percent + 1);

			start = find_percent; // ex) abc%id?

			if (end_ == std::string::npos) {
				end_ = option_str.size();
			}

			const std::string opt = option_str.substr(start, end_ - 1 - start + 1);

			if ("%int" == opt) {
				option.Type(Option::Type_::INT);
			}
			else if ("%float" == opt) {
				option.Type(Option::Type_::FLOAT);
			}
			else if ("%quoted_string" == opt) {
				option.Type(Option::Type_::QUOTED_STRING);
			}
			else if ("%string" == opt) {
				option.Type(Option::Type_::STRING);
			}
			else if ("%datetime" == opt) {
				option.Type(Option::Type_::DATETIME);
			}
			else if ("%datetime_a" == opt) {
				option.Type(Option::Type_::DATETIME_A);
			}
			else if ("%datetime_b" == opt) {
				option.Type(Option::Type_::DATETIME_B);
			}
			else if ("%id" == opt) {
				option.Id(Option::Id_::ID);
			}
			else if ("%total_id" == opt) {
				option.Id(Option::Id_::TOTAL_ID);
			}
			else if ("%any" == opt) {
				option.Type(Option::Type_::ANY);
			}
			else if ("%one_more" == opt) { // x = { 1 2 3 4  } -> x = { %int%one_more%event_plus_test }
				option.OneMore(Option::OneMore_::ONEMORE);
			}
			else if ("%just_one" == opt) {
				option.OneMore(Option::OneMore_::JUSTONE);
			}
			else if ("%optional" == opt) {
				option.Required(Option::Required_::OPTIONAL_);
			}
			else if ("%required" == opt) {
				option.Required(Option::Required_::REQUIRED);
			}
			else if ("%can_empty_ut" == opt) {
				option.EmptyUT(Option::EmptyUT_::ON);
			}
			else if (wiz::String::startsWith(opt, "%event_")) { // size check?
				std::string event_name = wiz::String::substring(opt, 7);
				option.Event(std::move(event_name));
			}
			else {
				std::cout << "wrong option" << ENTER;
				exit(-2);
			}

			start = end_;
		}

		if (option.type.empty()) {
			option.type.push_back(Option::Type_::ANY);
		}
		return option;
	}

	// chk valid in here!
	inline bool OptionDoA(const Option & option, const std::string & str)
	{
		if (option.prefix.empty() == false &&
			option.prefix == str) {
			return true;
		}
		else if (option.prefix.empty() == false &&
			option.prefix != str) {
			return false;
		}

		long long count = option.type.size();
		auto type_list = option.type;

		while (!type_list.empty()) {
			switch (type_list.back()) {
			case Option::Type_::ANY:
				// pass
				break;
			case Option::Type_::INT:
				if (wiz::load_data::Utility::IsInteger(str)) {
					//
				}
				else {
					std::cout << str << " is not integer" << ENTER;
					count--;
				}
				break;
			case Option::Type_::FLOAT:
				if (wiz::load_data::Utility::IsDouble(str)) {
					//
				}
				else {
					std::cout << str << " is not double" << ENTER;
					count--;
				}
				break;
			case Option::Type_::DATETIME:
				if (wiz::load_data::Utility::IsDate(str)) {
					//
				}
				else {
					std::cout << str << " is not date" << ENTER;
					count--;
				}
				break;
			case Option::Type_::DATETIME_A:
				if (wiz::load_data::Utility::IsDateTimeA(str)) {
					//
				}
				else {
					std::cout << str << " is not datetime A" << ENTER;
					count--;
				}
				break;
			case Option::Type_::DATETIME_B:
				if (wiz::load_data::Utility::IsDateTimeB(str)) {
					//
				}
				else {
					std::cout << str << " is not datetime B" << ENTER;
					count--;
				}
				break;
			case Option::Type_::QUOTED_STRING:
				if (str.size() >= 2 && str[0] == str.back() && str.back() == '\"') { // cf '\''
					//
				}
				else {
					std::cout << str << " is not quoted string" << ENTER;
					count--;
				}
				break;
			case Option::Type_::STRING:
				// pass
				break;
			}
			type_list.pop_back();
		}

		return count > 0;
	}

	std::tuple<bool, Option, Option> _Check(wiz::load_data::UserType * schema_eventUT,
		const wiz::load_data::ItemType<WIZ_STRING_TYPE> & x, const wiz::load_data::ItemType<WIZ_STRING_TYPE> & y, const std::string & real_dir) //, Order?
	{
		const Option var_option = OptionFrom(x.GetName().ToString()); // name, value check - not start with % ??
		const Option val_option = OptionFrom(x.Get(0).ToString());

		// val only case, ex) A = { a b c d } , a, b, c, d `s name is empty.
		if (x.GetName().ToString().empty()) {
			if (!y.GetName().ToString().empty()) {
				//
				return{ false, var_option, val_option };
			}
		}
		else { // bug?
			if (y.GetName().ToString().empty()) {
				//
				return{ false, var_option, val_option };
			}
		}

		// option type check.
		const bool name_do = OptionDoA(var_option, y.GetName().ToString());
		const bool val_do = OptionDoA(val_option, y.Get(0).ToString());

		if (name_do && val_do) {
			// event check.
			wiz::ClauText clauText;
			wiz::StringTokenizer tokenizer(var_option.event_ids, &builder);
			wiz::StringTokenizer tokenizer2(val_option.event_ids, &builder);
			std::string event_name;

			while (tokenizer.hasMoreTokens()) {
				event_name = tokenizer.nextToken();

				// for var // chk no start with __, no end with __ ?
				wiz::load_data::LoadData::AddData(*schema_eventUT, "/./",
					"Event = { id = __" + event_name + "__ $call = { id = " + event_name +
					" name = " + y.GetName().ToString() + " value = " + y.Get(0).ToString() +
					" is_usertype = FALSE " +
					" real_dir = " + real_dir +
					" select = NAME " +
					" input = " + y.GetName().ToString() +
					" } }",
					"TRUE", wiz::ExcuteData(), &builder);

				if ("TRUE" == clauText.excute_module("Main = { $call = { id = __" + event_name + "__ } }", schema_eventUT, wiz::ExcuteData(), opt, 1)) {
					schema_eventUT->RemoveUserTypeList(schema_eventUT->GetUserTypeListSize() - 1);
				}
				else {
					schema_eventUT->RemoveUserTypeList(schema_eventUT->GetUserTypeListSize() - 1);

					std::cout << "clauText is not valid1" << ENTER;
					return { false, var_option, val_option };
				}
			}
			while (tokenizer2.hasMoreTokens()) {
				event_name = tokenizer2.nextToken();
				// for val
				wiz::load_data::LoadData::AddData(*schema_eventUT, "/./",
					"Event = { id = __" + event_name + "__ $call = { id = " + event_name +
					" name = " + y.GetName().ToString() + " value = " + y.Get(0).ToString() +
					" is_usertype = FALSE " +
					" real_dir = " + real_dir +
					" select = VALUE " +
					" input = " + y.Get(0).ToString() +
					" } }",
					"TRUE", wiz::ExcuteData(), &builder);

				if ("TRUE" == clauText.excute_module("Main = { $call = { id = __" + event_name + "__ } }", schema_eventUT, wiz::ExcuteData(), opt, 1)) {
					schema_eventUT->RemoveUserTypeList(schema_eventUT->GetUserTypeListSize() - 1);
				}
				else {
					schema_eventUT->RemoveUserTypeList(schema_eventUT->GetUserTypeListSize() - 1);

					std::cout << "clauText is not valid2" << ENTER;
					return { false, var_option, val_option };
				}
			}
		}
		else {
			return { false, var_option, val_option };
		}

		return { true, var_option, val_option };
	}

	std::tuple<bool, Option> _Check(wiz::load_data::UserType * schema_eventUT,
		const wiz::load_data::UserType * x, const wiz::load_data::UserType * y, const std::string & real_dir // Order?
	)
	{
		Option var_option = OptionFrom(x->GetName().ToString()); // name, value check - not start with % ??

		// val only case, ex) A = { a b c d } , a, b, c, d `s name is empty.
		if (x->GetName().ToString().empty()) {
			if (!y->GetName().ToString().empty()) {
				//
				return{ false, var_option };
			}
		}
		else { // bug?
			if (!y || y->GetName().ToString().empty()) {
				//
				return{ false, var_option };
			}
		}

		// option type check.
		const bool name_do = OptionDoA(var_option, y->GetName().ToString());

		if (name_do) {
			// event check.
			wiz::ClauText clauText;
			wiz::StringTokenizer tokenizer(var_option.event_ids, &builder);
			std::string event_name;

			while (tokenizer.hasMoreTokens()) {
				event_name = tokenizer.nextToken();

				// for var // chk no start with __, no end with __ ?
				wiz::load_data::LoadData::AddData(*schema_eventUT, "/./",
					"Event = { id = __" + event_name + "__ $call = { id = " + event_name +
					" name = " + y->GetName().ToString() +
					" is_usertype = TRUE " +
					" real_dir = " + real_dir +
					" select = NAME " +
					" input = " + y->GetName().ToString() +
					"}  } ",
					"TRUE", wiz::ExcuteData(), &builder);

				if ("TRUE" == clauText.excute_module("Main = { $call = { id = __" + event_name + "__ } }", schema_eventUT, wiz::ExcuteData(), opt, 1)) {
					schema_eventUT->RemoveUserTypeList(schema_eventUT->GetUserTypeListSize() - 1);
				}
				else {
					schema_eventUT->RemoveUserTypeList(schema_eventUT->GetUserTypeListSize() - 1);

					std::cout << "clauText is not valid3" << ENTER;
					return { false, var_option };
				}
			}
		}
		else {
			return { false, var_option };
		}

		return { true, var_option };
	}

	// varaible!
	std::set<std::tuple<std::string, std::string, std::string>> check_total_id;

	bool Check(wiz::load_data::UserType* schema_eventUT, wiz::load_data::UserType* schemaUT,
		wiz::load_data::UserType* clautextUT, int depth, bool& log_on, bool is_optional = false)
	{
		bool use_it_order_on = false;
		bool use_ut_order_on = false;

		Option::Order_ order = Option::Order_::OFF;
		long long ct_itCount = 0; // for clautextUT?
		long long ct_utCount = 0; // for clautextUT?
		long long itCount = 0;
		long long utCount = 0;

		long multiple_flag = 0; // 0 : no multiple, 1 : multiple

		// for ORDER_::OFF
		std::vector<bool> validVisit(schemaUT->GetIListSize(), false);
		std::vector<Option> varOptionVisit(schemaUT->GetIListSize()); // remove?
		std::vector<bool> mark(clautextUT->GetItemListSize(), false); // ct_it
		std::vector<bool> mark2(clautextUT->GetUserTypeListSize(), false); // ct_ut

		std::set<std::pair<std::string, std::string>> check_id;

		int multiple_run = 0; // 1 - it(itemtype) run, 2 - ut(usertype) run.

		long long count_ct_it = 0;
		long long count_ct_ut = 0;


		for (long long i = 0; i < schemaUT->GetIListSize(); ++i)
		{
			if (depth == 0) { // chk - clau`s depth >= 1 ( { ~~ } )
				check_total_id.clear();
			}
			const bool chk_ct_it = ct_itCount < clautextUT->GetItemListSize();
			const bool chk_ct_ut = ct_utCount < clautextUT->GetUserTypeListSize();

			bool check_pass = false;

			if (schemaUT->IsItemList(i)) {
				if (schemaUT->GetItemListSize() > 0 && schemaUT->GetItemList(itCount).ToString() == "%order_on") {
					order = Option::Order_::ON;
					validVisit[i] = true;
					itCount++;

					if (use_it_order_on) {
						ct_itCount += count_ct_it;
						count_ct_it = 0;
					}
					if (use_ut_order_on) {
						ct_utCount += count_ct_ut;
						count_ct_ut = 0;
					}

					use_it_order_on = false;
					use_ut_order_on = false;

					continue;
				}
				else if (schemaUT->GetItemList(itCount).ToString() == "%order_off") {
					order = Option::Order_::OFF;

					if (use_it_order_on) {
						ct_itCount += count_ct_it;
						count_ct_it = 0;
					}
					if (use_ut_order_on) {
						ct_utCount += count_ct_ut;
						count_ct_ut = 0;
					}


					use_it_order_on = false;
					use_ut_order_on = false;

					validVisit[i] = true;
					itCount++;
					continue;
				}
				else if (schemaUT->GetItemList(itCount).ToString() == "%multiple_on") {
				//	if (order == Option::Order_::OFF) {
				//		std::cout << "to do %multple_on, need to %order_on!" << ENTER;
				//		throw "ERROR1";
				//	}
					multiple_flag = 1;
					validVisit[i] = true;
					itCount++;
					continue;
				}
				else if (schemaUT->GetItemList(itCount).ToString() == "%multiple_off") {
					//if (order == Option::Order_::OFF) {
					//	std::cout << "to do %multple_off, need to %order_on!" << ENTER;
					//	throw "ERROR2";
					//}
					multiple_flag = 0;
					multiple_run = 0;
					validVisit[i] = true;
					itCount++;
					continue;
				}
				else if (schemaUT->GetItemList(itCount).ToString() == "%log_on") {
					log_on = true;
					validVisit[i] = true;
					itCount++;
					continue;
				}
				else if (schemaUT->GetItemList(itCount).ToString() == "%log_off") {
					log_on = false;
					validVisit[i] = true;
					itCount++;
					continue;
				}


				// log
				if (log_on) {
					std::cout << ENTER << "<itemtype> ";
					std::cout << "[depth] : " << depth << " ";
					std::cout << "[~th] : " << itCount << " ";
					std::cout << "[schema] : " << schemaUT->GetItemList(itCount).ToString() << " ";
				}



				// off -> no order? : slow??
				if (order == Option::Order_::OFF) {
					bool pass = false;
					bool use_onemore = false;
					std::tuple<bool, Option, Option> temp;

					int check_justone = 0;

					int count_iter = 0;

					// schemaUT?
					for (long long j = 0; j < clautextUT->GetItemListSize(); ++j) {
						if (count_iter > 0 && 0 == multiple_flag) {
							break;
						}

						if (log_on) {
							std::cout << ENTER << "\t" << "[clau ~th] : " << j << " "
								<< "[clautext] : " << clautextUT->GetItemList(j).ToString() << ENTER;
						}

						if (mark[j] == true) {
							continue;
						}

						temp = _Check(schema_eventUT, schemaUT->GetItemList(itCount), clautextUT->GetItemList(j), wiz::load_data::LoadData::GetRealDir(clautextUT->GetItemList(j).GetName().ToString(), clautextUT, &builder));


						if (mark[j] == false &&
							std::get<0>(temp)
							) {
							count_iter++;

							// visit vector? chk?
							validVisit[i] = true;
							varOptionVisit[i] = std::get<1>(temp);
							mark[j] = true;

							pass = true;

							// check id, total id! 
							if (std::get<1>(temp).id == Option::Id_::ID) {
								const std::string key_1 = clautextUT->GetItemList(j).GetName().ToString();
								const std::string key_2 = "it_name";
								const std::pair<std::string, std::string> key(key_1, key_2);

								if (check_id.find(key) == check_id.end()) {
									check_id.insert(key);
								}
								else {
									std::cout << "clauText is not valid - ID1" << ENTER;
									return false;
								}
							}
							else if (std::get<1>(temp).id == Option::Id_::TOTAL_ID) {
								if (0 == depth) {
									std::cout << "ERROR schema is not valid1" << ENTER;
									return false;
								}
								const std::string key_1 = clautextUT->GetItemList(j).GetName().ToString();
								const std::string key_2 = wiz::load_data::LoadData::GetRealDir(schemaUT->GetName().ToString(), schemaUT, &builder);
								const std::string key_3 = "it_name";

								std::tuple<std::string, std::string, std::string> key(key_1, key_2, key_3);

								if (check_total_id.find(key) == check_total_id.end()) {
									check_total_id.insert(key);
								}
								else {
									std::cout << "clauText is not valid _ ID2" << ENTER;
									return false;
								}
							}
							if (std::get<2>(temp).id == Option::Id_::ID) {
								const std::string key_1 = clautextUT->GetItemList(j).Get(0).ToString();
								const std::string key_2 = "it_value";
								const std::pair<std::string, std::string> key(key_1, key_2);

								if (check_id.find(key) == check_id.end()) {
									check_id.insert(key);
								}
								else {
									std::cout << "clauText is not valid - ID3" << ENTER;
									return false;
								}
							}
							else if (std::get<2>(temp).id == Option::Id_::TOTAL_ID) {
								if (0 == depth) {
									std::cout << "ERROR schema is not valid2" << ENTER;
									return false;
								}

								const std::string key_1 = clautextUT->GetItemList(j).Get(0).ToString();
								const std::string key_2 = wiz::load_data::LoadData::GetRealDir(schemaUT->GetName().ToString(), schemaUT, &builder);
								const std::string key_3 = "it_value";

								std::tuple<std::string, std::string, std::string> key(key_1, key_2, key_3);

								if (check_total_id.find(key) == check_total_id.end()) {
									check_total_id.insert(key);
								}
								else {
									std::cout << "clauText is not valid _ ID4" << ENTER;
									return false;
								}
							}

							// check justone, (onemore)
							if (std::get<1>(temp).onemore == Option::OneMore_::JUSTONE) { // justone -> only for name! , not for value!
								if (check_justone > 0) {
									std::cout << "clauText is not valid, justone is set, but not justone. 1" << ENTER;
									return false;
								}
								else {
									check_justone++;
									count_ct_it++;
									use_it_order_on = true;
								}
							}
							else {
								use_it_order_on = true;
								count_ct_it++; //ct_itCount++;
								use_onemore = true;
							}
						}
					}

					if (false == pass) {
						Option var_option = OptionFrom(schemaUT->GetItemList(itCount).GetName().ToString());

						if (var_option.required == Option::Required_::OPTIONAL_) { // optional -> only for name
							validVisit[i] = true;
						}
						else {
							std::cout << "clauText is not valid4" << ENTER;
							return false;
						}
					}
				}
				else if (order == Option::Order_::ON) {
					if (!chk_ct_it) {
						std::cout << "chk_ct_it is false" << ENTER;

						if (1 == multiple_run) {
							itCount++;
							continue;
						}
						else {
							break;
						}
					}

					if (log_on) {
						std::cout << "[clau ~th] : " << ct_itCount << " "
							<< "[clautext] : " << clautextUT->GetItemList(ct_itCount).ToString() << ENTER;
					}

					check_pass = true;

					int check_justone = 0;

					std::tuple<bool, Option, Option> temp;
					temp = _Check(schema_eventUT, schemaUT->GetItemList(itCount), clautextUT->GetItemList(ct_itCount), wiz::load_data::LoadData::GetRealDir(clautextUT->GetItemList(ct_itCount).GetName().ToString(), clautextUT, &builder));

					if (mark[ct_itCount]) {
						std::cout << "mark " << ENTER;
						return false;
					}
					else if (std::get<0>(temp)) {
						validVisit[i] = true;

						// check id, total id!
						if (std::get<1>(temp).id == Option::Id_::ID) {
							const std::string key_1 = clautextUT->GetItemList(ct_itCount).GetName().ToString();
							const std::string key_2 = "it_name";
							const std::pair<std::string, std::string> key(key_1, key_2);

							if (check_id.find(key) == check_id.end()) {
								check_id.insert(key);
							}
							else {
								std::cout << "clauText is not valid - ID5" << ENTER;
								return false;
							}
						}
						else if (std::get<1>(temp).id == Option::Id_::TOTAL_ID) {
							if (0 == depth) {
								std::cout << "ERROR schema is not valid3" << ENTER;
								return false;
							}

							const std::string key_1 = clautextUT->GetItemList(ct_itCount).GetName().ToString();
							const std::string key_2 = wiz::load_data::LoadData::GetRealDir(schemaUT->GetName().ToString(), schemaUT, &builder);
							const std::string key_3 = "it_name";

							std::tuple<std::string, std::string, std::string> key(key_1, key_2, key_3);

							if (check_total_id.find(key) == check_total_id.end()) {
								check_total_id.insert(key);
							}
							else {
								std::cout << "clauText is not valid _ ID6" << ENTER;
								return false;
							}
						}

						if (std::get<2>(temp).id == Option::Id_::ID) {
							const std::string key_1 = clautextUT->GetItemList(ct_itCount).Get(0).ToString();
							const std::string key_2 = "it_value";
							const std::pair<std::string, std::string> key(key_1, key_2);

							if (check_id.find(key) == check_id.end()) {
								check_id.insert(key);
							}
							else {
								std::cout << "clauText is not valid - ID7" << ENTER;
								return false;
							}
						}
						else if (std::get<2>(temp).id == Option::Id_::TOTAL_ID) {
							if (0 == depth) {
								std::cout << "ERROR schema is not valid4" << ENTER; // use id!
								return false;
							}

							const std::string key_1 = clautextUT->GetItemList(ct_itCount).Get(0).ToString();
							const std::string key_2 = wiz::load_data::LoadData::GetRealDir(schemaUT->GetName().ToString(), schemaUT, &builder);
							const std::string key_3 = "it_value";

							std::tuple<std::string, std::string, std::string> key(key_1, key_2, key_3);

							if (check_total_id.find(key) == check_total_id.end()) {
								check_total_id.insert(key);
							}
							else {
								std::cout << "clauText is not valid _ ID8" << ENTER;
								return false;
							}
						}

						// check justone, (onemore)
						if (std::get<1>(temp).onemore == Option::OneMore_::JUSTONE) { // justone -> only for name! , not for value!
							//
						}
						else if (std::get<1>(temp).onemore == Option::OneMore_::ONEMORE) {
							std::cout << "clauText is not valid, in order_on no onemore! 1" << ENTER;
							return false;
						}
						else {
							//
						}
					}
					else if (std::get<1>(temp).required == Option::Required_::OPTIONAL_) {
						if (i == schemaUT->GetIListSize() - 1) {
							throw "clauText is not valid - error1 ";
						}

						check_pass = false;
						validVisit[i] = true;

						if (1 == multiple_flag && itCount < schemaUT->GetItemListSize() - 1 &&
							schemaUT->GetItemList(itCount + 1).ToString() == "%multiple_off") {
							multiple_flag = 0;
							multiple_run = 0;
						}
					}
					else if (1 == multiple_flag && itCount < schemaUT->GetItemListSize() - 1 &&
						schemaUT->GetItemList(itCount + 1).ToString() == "%multiple_off") {
						multiple_flag = 0;
						multiple_run = 0;
						check_pass = false;
					}
					else {
						std::cout << "clauText is not valid6" << ENTER;
						return false;
					}

					if (1 == multiple_flag) {
						multiple_run = 1;
						itCount--; i--;
					}
				}

				if (Option::Order_::ON == order && check_pass) {
					ct_itCount++;
				}
				itCount++;
			}
			else { // usertype
				// log
				if (log_on) {
					std::cout << ENTER << "<usertype> ";
					std::cout << "[depth] : " << depth << " ";
					std::cout << "[~th] : " << utCount << " ";
					std::cout << "[schema] : " << schemaUT->GetUserTypeList(utCount)->GetName().ToString() << " ";
				}

				// off -> no order? : slow??
				if (order == Option::Order_::OFF) {
					bool pass = false;
					bool use_onemore = false;
					std::tuple<bool, Option> temp3;
					int count_iter = 0;
					int check_justone = 0;

					for (long long j = 0; j < clautextUT->GetUserTypeListSize(); ++j) {
						if (count_iter > 0 && 0 == multiple_flag) {
							break;
						}
						
						if (mark2[j]) {
							continue;
						}

						if (log_on) {
							std::cout << ENTER << "\t" << "[clau ~th] : " << j << " "
								<< "[clautext] : " << clautextUT->GetUserTypeList(j)->GetName().ToString() << ENTER;
						}

						temp3 = _Check(schema_eventUT, schemaUT->GetUserTypeList(utCount), clautextUT->GetUserTypeList(j), wiz::load_data::LoadData::GetRealDir(clautextUT->GetUserTypeList(j)->GetName().ToString(), clautextUT->GetUserTypeList(j), &builder));

						if (mark2[j] == false && (std::get<0>(temp3) || std::get<1>(temp3).required == Option::Required_::OPTIONAL_)) {
							count_iter++;

							if (log_on) {
								std::cout << " { " << ENTER;
							}
							if (std::get<1>(temp3).empty_ut == Option::EmptyUT_::ON && 0 == clautextUT->GetUserTypeList(j)->GetIListSize()) {
								validVisit[i] = true;
							}
							else if (Check(schema_eventUT, schemaUT->GetUserTypeList(utCount), clautextUT->GetUserTypeList(j), depth + 1, log_on, is_optional || std::get<1>(temp3).required == Option::Required_::OPTIONAL_)) {
								//
							}
							else if (std::get<1>(temp3).required == Option::Required_::OPTIONAL_) {
								validVisit[i] = true;
							}
							else {
								std::cout << "clauText is not valid8" << ENTER;
								return false;
							}

							if (log_on) {
								std::cout << " } " << ENTER;
							}

							// visit vector? chk?
							validVisit[i] = true;
							varOptionVisit[i] = std::get<1>(temp3);
							mark2[j] = true;

							pass = true;

							// check id, total id!
							if (std::get<1>(temp3).id == Option::Id_::ID) {
								const std::string key_1 = clautextUT->GetUserTypeList(j)->GetName().ToString();
								const std::string key_2 = "ut_name";
								const std::pair<std::string, std::string> key(key_1, key_2);

								if (check_id.find(key) == check_id.end()) {
									check_id.insert(key);
								}
								else {
									std::cout << "clauText is not valid - ID9" << ENTER;
									return false;
								}
							}
							else if (std::get<1>(temp3).id == Option::Id_::TOTAL_ID) {
								if (0 == depth) {
									std::cout << "ERROR schema is not valid5" << ENTER;
									return false;
								}

								const std::string key_1 = clautextUT->GetUserTypeList(j)->GetName().ToString();
								const std::string key_2 = wiz::load_data::LoadData::GetRealDir(schemaUT->GetName().ToString(), schemaUT, &builder);
								const std::string key_3 = "ut_name";

								std::tuple<std::string, std::string, std::string> key(key_1, key_2, key_3);

								if (check_total_id.find(key) == check_total_id.end()) {
									check_total_id.insert(key);
								}
								else {
									std::cout << "clauText is not valid _ ID10" << ENTER;
									return false;
								}
							}

							// check justone, (onemore)
							if (std::get<1>(temp3).onemore == Option::OneMore_::JUSTONE) { // justone -> only for name! , not for value!
								if (check_justone > 0) {

									std::cout << "clauText is not valid, justone is set, but not justone. 3" << ENTER;
									return false;
								}
								else {
									check_justone++;
									use_ut_order_on = true;
									count_ct_ut++; //ct_utCount++;
									use_onemore = true;
								}
							}
							else {
								use_ut_order_on = true;
								count_ct_ut++; //ct_utCount++;
							}
						}
					}
					if (false == pass) {
						Option var_option = OptionFrom(schemaUT->GetUserTypeList(utCount)->GetName().ToString());

						if (var_option.required == Option::Required_::OPTIONAL_) {
							validVisit[i] = true;
							utCount++;
							break;
						}
						else {
							std::cout << "clauText is not valid9 " << schemaUT->GetUserTypeList(utCount)->GetName().ToString() << ENTER;
							return false;
						}
					}
				}
				else if (order == Option::Order_::ON) {
					if (!chk_ct_ut)
					{
						std::cout << "chk_ct_ut is false" << ENTER;

						std::tuple<bool, Option> temp = _Check(schema_eventUT,
							schemaUT->GetUserTypeList(utCount), nullptr, "");

						is_optional = std::get<1>(temp).required == Option::Required_::OPTIONAL_;


						if (2 == multiple_run) {
							utCount++;
							continue;
						}
						else {
							return is_optional; //... optional..... - ToDo : removal??
						}
					}

					if (log_on) {
						std::cout << "[clau ~th] : " << ct_utCount << " "
							<< "[clautext] : " << clautextUT->GetUserTypeList(ct_utCount)->GetName().ToString() << ENTER;
					}

					check_pass = true;
					int check_justone = 0;


					std::tuple<bool, Option> temp = _Check(schema_eventUT,
						schemaUT->GetUserTypeList(utCount), clautextUT->GetUserTypeList(ct_utCount),
						wiz::load_data::LoadData::GetRealDir(clautextUT->GetUserTypeList(ct_utCount)->GetName().ToString(), clautextUT->GetUserTypeList(ct_utCount), &builder)
					);

					if (mark2[ct_utCount]) {
						std::cout << "mark2" << ENTER;
						return false;
					}
					else if (std::get<0>(temp)) {
						if (log_on) {
							std::cout << " { " << ENTER;
						}


						if (std::get<1>(temp).empty_ut == Option::EmptyUT_::ON && 0 == clautextUT->GetUserTypeList(ct_utCount)->GetIListSize()) {
							check_pass = true;
							validVisit[i] = true;
						}
						else if (Check(schema_eventUT, schemaUT->GetUserTypeList(utCount), clautextUT->GetUserTypeList(ct_utCount), depth + 1, log_on, is_optional || std::get<1>(temp).required == Option::Required_::OPTIONAL_)) {
							validVisit[i] = true;

							// check id, total id!
							if (std::get<1>(temp).id == Option::Id_::ID) {
								const std::string key_1 = clautextUT->GetUserTypeList(ct_utCount)->GetName().ToString();
								const std::string key_2 = "ut_name";
								const std::pair<std::string, std::string> key(key_1, key_2);

								if (check_id.find(key) == check_id.end()) {
									check_id.insert(key);
								}
								else {
									std::cout << "clauText is not valid - ID11" << ENTER;
									return false;
								}
							}
							else if (std::get<1>(temp).id == Option::Id_::TOTAL_ID) {
								if (0 == depth) {
									std::cout << "ERROR schema is not valid6" << ENTER;
									return false;
								}

								const std::string key_1 = clautextUT->GetUserTypeList(ct_utCount)->GetName().ToString();
								const std::string key_2 = wiz::load_data::LoadData::GetRealDir(schemaUT->GetName().ToString(), schemaUT, &builder);
								const std::string key_3 = "ut_name";

								std::tuple<std::string, std::string, std::string> key(key_1, key_2, key_3);

								if (check_total_id.find(key) == check_total_id.end()) {
									check_total_id.insert(key);
								}
								else {
									std::cout << "clauText is not valid _ ID12" << ENTER;
									return false;
								}
							}

							// check justone, (onemore)
							if (std::get<1>(temp).onemore == Option::OneMore_::JUSTONE) { // justone -> only for name! , not for value!
								//
							}
							else if (std::get<1>(temp).onemore == Option::OneMore_::ONEMORE) {
								std::cout << "clauText is not valid, in order_on no onemore! 2" << ENTER;
								return false;
							}
							else {
								//
							}
						}
						else if (std::get<1>(temp).required == Option::Required_::OPTIONAL_) {
							if (i == schemaUT->GetIListSize() - 1) {
								throw "clauText is not valid - error3 ";
							}
							check_pass = false;
							validVisit[i] = true;
						}
						else {
							std::cout << "clauText is not valid11" << ENTER;
							return false;
						}
						if (log_on) {
							std::cout << " } " << ENTER;
						}
					}
					else if (std::get<1>(temp).required == Option::Required_::OPTIONAL_) {
						if (i == schemaUT->GetIListSize() - 1) {
							throw "clauText is not valid - error4 ";
						}

						check_pass = false;
						validVisit[i] = true;

						if (1 == multiple_flag &&
							schemaUT->GetItemList(itCount).ToString() == "%multiple_off") {
							multiple_flag = 0;
							multiple_run = 0;
							//ct_utCount--;
						}
					}
					else if (1 == multiple_flag &&
						schemaUT->GetItemList(itCount).ToString() == "%multiple_off") {
						multiple_flag = 0;
						multiple_run = 0;
						check_pass = false;
					}
					else {
						std::cout << "clauText is not valid12" << ENTER;
						return false;
					}

					if (1 == multiple_flag) {
						multiple_run = 2;
						utCount--;
						i--;
					}
				}
				if (Option::Order_::ON == order && check_pass) {
					ct_utCount++;
				}
				utCount++;
			}
		}


		if (use_it_order_on) { // order_off?
			ct_itCount += count_ct_it;
		}
		if (use_ut_order_on) {
			ct_utCount += count_ct_ut;
		}


		if (multiple_flag && 2 == multiple_run) {
			//utCount++;
		}
		else if (multiple_flag && 1 == multiple_run) {
			//itCount++;
		}
		else if (multiple_flag) {
			std::cout << "multiple_flag is wrong.. " << ENTER;
			return false;
		}

		if (ct_itCount != clautextUT->GetItemListSize()) {
			std::cout << "clauText is not valid13 : " << ct_itCount << " " << clautextUT->GetItemListSize() << ENTER;
			return false;
		}
		if (ct_utCount != clautextUT->GetUserTypeListSize()) {
			std::cout << "clauText is not valid14 : " << ct_utCount << " " << clautextUT->GetUserTypeListSize() << ENTER;
			return false;
		}

		if (itCount != schemaUT->GetItemListSize()) {
			std::cout << "clauText is not valid15 : " << itCount << ENTER;
			return false;
		}

		if (utCount != schemaUT->GetUserTypeListSize()) {
			std::cout << "clauText is not valid16 : " << utCount << " " << schemaUT->GetUserTypeListSize() << ENTER;
			return false;
		}

		for (long long i = 0; i < validVisit.size(); ++i) {
			if (false == validVisit[i]) {
				std::cout << "clauText is not valid17" << ENTER;
				return false;
			}
		}

		return true;
	}

	inline bool _Validate(const wiz::load_data::UserType & schemaFileUT)
	{
		// 1. $로 시작하는 것들을 따로 분리해서 별도의 UserType에 저장한다.
			// 1-1. $Event -> 따로 뽑아낸 다음 Event로 rename?
			// 1-2. $변수들 -> 따로 뽑아낸 다음 변수들로 rename?
		// 2. $로 시작하지는 않는 것들은 data check 내용이다?
			// 2-1. %로 시작하는 것을 찾아서 각각 수행한다. 
		wiz::load_data::UserType* clautextUT;
		wiz::load_data::UserType schema_eventUT;
		wiz::load_data::UserType schemaUT;
		int itemCount = 0;
		int utCount = 0;

		for (int i = 0; i < schemaFileUT.GetIListSize(); ++i) {
			if (schemaFileUT.IsItemList(i)) { // itemtype
				const std::string name = wiz::ToString(schemaFileUT.GetItemList(itemCount).GetName());
				if (name.size() > 1 && name[0] == '$') {
					schema_eventUT.AddItem(name.substr(1), schemaFileUT.GetItemList(itemCount).Get(0));
				}
				else {
					schemaUT.AddItem(name, schemaFileUT.GetItemList(itemCount).Get(0));
				}
				itemCount++;
			}
			else { // usertype
				const std::string name = wiz::ToString(schemaFileUT.GetUserTypeList(utCount)->GetName());
				wiz::load_data::UserType temp = *schemaFileUT.GetUserTypeList(utCount);

				if (name.size() > 1 && name[0] == '$') {
					temp.SetName(wiz::ToString(temp.GetName()).substr(1));
					schema_eventUT.AddUserTypeItem(std::move(temp));
				}
				else {
					schemaUT.AddUserTypeItem(std::move(temp));
				}
				utCount++;
			}
		}

		// __init__ first init.
		std::string start_name;
		{
			wiz::ClauText clauText;
			start_name = clauText.excute_module("Main = { $call = { id = __init__ } }", &schema_eventUT, wiz::ExcuteData(), opt, 1); // 0 (remove events) -> 1 (revoke events?)

			if (start_name.empty()) {
				std::cout << "__init__ must return data`s start? folder?" << ENTER;
				exit(-1);
			}
		}

		clautextUT = wiz::load_data::UserType::Find(&schema_eventUT, start_name, &builder).second[0];

		// for log?
		bool log_on = false;
		const bool chk = Check(&schema_eventUT, &schemaUT, clautextUT, 0, log_on);

		{
			wiz::ClauText clauText;

			clauText.excute_module("Main = { $call = { id = __end__ } }", &schema_eventUT, wiz::ExcuteData(), opt, 1); // 0 (remove events) -> 1 (revoke events?)
		}

		//// debug
		//std::cout << schema_eventUT.ToString() << ENTER
		//	<< schemaUT.ToString() << ENTER;
		return chk;
	}

	inline bool Validate(const wiz::load_data::UserType & schema)
	{
		return _Validate(schema);
	}


	// to do
	//std::pair<bool, std::string> MakeSchema(const std::string& clautext)
	//{
	//	// to do
	//}


	inline std::pair<bool, wiz::load_data::UserType> LoadFile(const std::string & fileName)
	{
		wiz::load_data::UserType ut;

		bool valid = wiz::load_data::LoadData::LoadDataFromFile5_2(fileName, ut, 0, 0);

		return { valid, (ut) };
	}
	inline std::pair<bool, wiz::load_data::UserType> LoadJsonFile(const std::string & fileName)
	{
		wiz::load_data::UserType ut;

		bool valid = wiz::load_data::LoadData::LoadDataFromFileFastJson(fileName, ut, 0, 0);

		return { valid, (ut) };
	}

	// todo SaveJsonFile? for Make Schema?
	inline bool SaveFile(const std::string & fileName, wiz::load_data::UserType * data)
	{
		wiz::load_data::LoadData::SaveWizDB(*data, fileName, "1");
		return true;
	}


}

int main(int argc, char* argv[])
{
	//auto chk_schema = Lint::LoadFile(
	//	"C:\\Users\\vztpv\\Desktop\\Clau\\ClauText\\json_test.txt"); //
		//argv[2]); // 3 -> 2
//	return 0;
	
	//char buffer[] = "wow = zzz";
	//wiz::load_data::UserType* x;
	//wiz::load_data::UserType ut;
	//ut.AddItem(std::string(buffer, 3), std::string(buffer+6, 3));
//	ut.AddUserTypeItem(wiz::load_data::UserType(""));
	//ut.AddUserTypeItem(wiz::load_data::UserType(""));
//	ut.GetUserTypeList(0)->Remove();

//	return 0;
	
	std::string option;
	wiz::load_data::UserType schema; // argv[3]?
	wiz::load_data::UserType clautext; // argv[2]
	std::string fileName; // to save

	try {
		// -v : version? - to do
		// -V : Validate.
		// -M : Make schema. // removal?
		if (argc == 3 && 0 == strcmp("-V", argv[1])) {
			option = "-V";

			//auto chk_clautext = Lint::LoadFile(argv[2]);
			auto chk_schema = Lint::LoadFile(
				//"C:\\Users\\vztpv\\Desktop\\Clau\\ClauText\\c.txt"); //
				argv[2]); // 3 -> 2

			if (chk_schema.first) {
				schema = std::move(chk_schema.second);
			}
			else {
				std::cout << "schema load fail" << ENTER;
				return 1;
			}
			//if (chk_clautext.first) {
			//	clautext = std::move(chk_clautext.second);
			//}
			//else {
			//	std::cout << "clautext load fail" << ENTER;
			//	return 2;
			//}
		}
		else if (argc == 4 && 0 == strcmp("-M", argv[1])) {
			option = "-M";

			auto chk_clautext = Lint::LoadFile(argv[2]);

			if (chk_clautext.first) {
				clautext = std::move(chk_clautext.second);
			}
			else {
				std::cout << "clautext load fail" << ENTER;
				return 3;
			}
			fileName = argv[3];
		}

		if (option == "-V") {
			auto chk = Lint::Validate(schema);
			if (chk) {
				std::cout << ENTER << "success" << ENTER;
			}
			else {
				std::cout << ENTER << "fail" << ENTER;
			}
		}
		else if (option == "-M") {
			//	auto chk = MakeSchema(clautext);
			//	if (chk.first) {
			//		schema = chk.second;

			//		if (SaveFile(fileName, schema)) {

			//		}
			//		else {
			//			std::cout << "save fail" << ENTER;
			//			return 4;
			//		}

			//		std::cout << "success" << ENTER;
			//	}
			//	else {
			//		std::cout << "fail" << ENTER;
			//	}
		}
		else {
			std::cout << "it is not valid option" << ENTER;
		}
	}
	catch (const char* str) {
		std::cout << str << "\n";
	}
	catch (...) {
		std::cout << "unknown error\n";
	}
	return 0;
}


