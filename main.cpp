

#include <iostream>
#include "wiz/ClauText.h"
#include <stack>
#include <queue>

using namespace std::literals;


namespace Lint {
	class Option
	{
	public:
		enum class Type_ { ANY, INT, FLOAT, QUOTED_STRING, STRING };
		enum class Required_ { REQUIRED, OPTIONAL_ };

		//	enum class Multiple_ { NONE, OFF, ON };
	public:
		std::vector<Type_> type;
		std::vector<std::string> ids;
		Required_ required;
		//Multiple_ multiple;

		std::vector<std::string> event_ids;
		std::vector<std::string> enum_ids;
		std::vector<std::string> style_ids;

		std::string prefix;

		wiz::DataType minimum, maximum;
		bool use_minimum = false, use_maximum = false;
		long long minItems = 0, maxItems = 0; // todo!
		bool use_minItems = false, use_maxItems = false; // todo!

		std::string regex;

		std::vector<std::string> needs;
	public:
		Option() : type(),
			required(Required_::REQUIRED)
			//,	multiple(Multiple_::OFF)
		{
			//
		}
	public:
		//	Option& Multiple(Multiple_ multiple) {
			//	this->multiple = multiple;
		//		return *this;
		//	}
		Option& Type(Type_ type) {
			this->type.push_back(type);
			return *this;
		}
		Option& Id(const std::string& id) {
			this->ids.push_back(id);
			return *this;
		}
		Option& Required(Required_ required) {
			this->required = required;
			return *this;
		}
		Option& Event(const std::string& event_id) {
			this->event_ids.push_back(event_id);
			return *this;
		}
		Option& Enum(const std::string& enum_id) {
			this->enum_ids.push_back(enum_id);
			return *this;
		}
		Option& Style(const std::string& style_id) {
			this->style_ids.push_back(style_id);
			return *this;
		}
	public:
		// check valid function?
		bool IsValid() const {
			// to do
			return false;
		}
	public:
		Option operator+(const Option& other) const {
			Option temp = *this;

			for (size_t i = 0; i < other.event_ids.size(); ++i) {
				temp.event_ids.push_back(other.event_ids[i]);
			}
			for (size_t i = 0; i < other.enum_ids.size(); ++i) {
				temp.enum_ids.push_back(other.enum_ids[i]);
			}
			for (size_t i = 0; i < other.style_ids.size(); ++i) {
				temp.style_ids.push_back(other.style_ids[i]);
			}
			for (size_t i = 0; i < other.ids.size(); ++i) {
				temp.ids.push_back(other.ids[i]);
			}

			//if (other.multiple == Multiple_::ON) {
			//	temp.multiple = Multiple_::ON;
			//}


			if (other.required == Required_::OPTIONAL_) {
				temp.required = Required_::OPTIONAL_;
			}
			for (const auto& x : other.type) {
				temp.type.push_back(x);
			}

			if (other.use_minimum) {
				temp.use_minimum = true;
				temp.minimum = other.minimum;
			}
			if (other.use_maximum) {
				temp.use_maximum = true;
				temp.maximum = other.maximum;
			}

			if (other.use_minItems) {
				temp.use_minItems = true;
				temp.minItems = other.minItems;
			}
			if (other.use_maxItems) {
				temp.use_maxItems = true;
				temp.maxItems = other.maxItems;
			}
			if (!other.regex.empty()) {
				temp.regex = other.regex;
			}

			for (size_t i = 0; i < other.needs.size(); ++i) {
				temp.needs.push_back(other.needs[i]);
			}

			return temp;
		}
	};


	// itemtype, usertype flag?
	Option OptionFrom(const std::string& option_str, wiz::load_data::UserType* mainUT, const std::map<std::string, wiz::load_data::UserType*>& styleMap)
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

			if (wiz::String::startsWith(opt, "%int"sv)) {
				option.Type(Option::Type_::INT);

				std::vector<std::string> temp = wiz::tokenize(opt, '@');

				if (temp.size() > 1) {
					std::string argument = temp[1].substr(1, temp[1].size() - 2);
					wiz::load_data::UserType ut;
					wiz::load_data::LoadData::LoadDataFromString(argument, ut);

					auto item1 = ut.GetItem("minimum");
					if (item1.size() > 0) {
						option.use_minimum = true;
						option.minimum = item1[0].Get();
					}
					auto item2 = ut.GetItem("maximum");
					if (item2.size() > 0) {
						option.use_maximum = true;
						option.maximum = item2[0].Get();
					}
				}
			}
			else if (wiz::String::startsWith(opt, "%float"sv)) {
				option.Type(Option::Type_::FLOAT);

				std::vector<std::string> temp = wiz::tokenize(opt, '@');

				if (temp.size() > 1) {
					std::string argument = temp[1].substr(1, temp[1].size() - 2);
					wiz::load_data::UserType ut;
					wiz::load_data::LoadData::LoadDataFromString(argument, ut);

					auto item1 = ut.GetItem("minimum");
					if (item1.size() > 0) {
						option.use_minimum = true;
						option.minimum = item1[0].Get();
					}
					auto item2 = ut.GetItem("maximum");
					if (item2.size() > 0) {
						option.use_maximum = true;
						option.maximum = item2[0].Get();
					}
				}
			}
			else if ("%quoted_string"sv == opt) {
				option.Type(Option::Type_::QUOTED_STRING);
			}
			else if ("%string"sv == opt) {
				option.Type(Option::Type_::STRING);
			}
			else if (wiz::String::startsWith(opt, "%id_")) {
				option.Id(wiz::String::substring(opt, 4));
			}
			else if ("%any"sv == opt) {
				option.Type(Option::Type_::ANY);
			}
			else if ("%optional"sv == opt) {
				option.Required(Option::Required_::OPTIONAL_);
			}
			else if ("%required"sv == opt) {
				option.Required(Option::Required_::REQUIRED);
			}
			//	else if ("%multiple"sv == opt) {
			//		option.Multiple(Option::Multiple_::ON);
			//	}
			else if (wiz::String::startsWith(opt, "%minItems"sv)) { // size check?
				std::string argument;

				{
					std::vector<std::string> temp = wiz::tokenize(opt, '@');
					if (temp.size() > 1) {
						argument = temp[1].substr(1, temp[1].size() - 2);

						wiz::DataType x(argument);
						option.use_minItems = true;
						option.minItems = x.ToInt();
					}
				}

			}
			else if (wiz::String::startsWith(opt, "%maxItems"sv)) { // size check?
				std::string argument;

				{
					std::vector<std::string> temp = wiz::tokenize(opt, '@');
					if (temp.size() > 1) {
						argument = temp[1].substr(1, temp[1].size() - 2);

						wiz::DataType x(argument);
						option.use_maxItems = true;
						option.maxItems = x.ToInt();
					}
				}
			}
			else if (wiz::String::startsWith(opt, "%regex"sv)) {
				std::string expr;

				std::vector<std::string> temp = wiz::tokenize(opt, '@');
				if (temp.size() > 1) {
					expr = temp[1].substr(1, temp[1].size() - 2);

					option.regex = expr;
				}
			}
			else if (wiz::String::startsWith(opt, "%need"sv)) {
				std::string name;

				std::vector<std::string> temp = wiz::tokenize(opt, '@');

				if (temp.size() > 1) {
					name = temp[1].substr(1, temp[1].size() - 2);

					option.needs.push_back(name);
				}
			}
			else if (wiz::String::startsWith(opt, "%event_"sv)) { // size check?
				std::string event_name = wiz::String::substring(opt, 7);
				option.Event(std::move(event_name));
			}
			else if (wiz::String::startsWith(opt, "%enum_"sv)) { // size check?
				std::string enum_name = wiz::String::substring(opt, 6);
				option.Enum(std::move(enum_name));
			}
			else if (wiz::String::startsWith(opt, "%style_"sv)) {
				std::string style_id;
				std::string argument;
				wiz::ExecuteData executeData;

				{
					std::vector<std::string> temp = wiz::tokenize(opt.substr(7), '@');
					style_id = temp[0];
					if (temp.size() > 1) {
						argument = temp[1].substr(1, temp[1].size() - 2);
					}

					wiz::DataType x = wiz::load_data::LoadData::ToBool4(nullptr, *mainUT, argument, wiz::ExecuteData()).ToString();
					wiz::load_data::UserType ut;
					wiz::load_data::LoadData::LoadDataFromString(x.ToString(), ut);

					for (size_t i = 0; i < ut.GetItemListSize(); ++i) {
						executeData.info.parameters.insert({ ut.GetItemList(i).GetName(), ut.GetItemList(i).Get() });
					}
				}

				std::string result;

				auto styleUT = styleMap.at(style_id);

				for (size_t i = 0; i < styleUT->GetItemListSize(); ++i) {
					result += styleUT->GetItemList(i).Get();
				}

				for (size_t i = 0; i < styleUT->GetUserTypeListSize(); ++i) {
					if (styleUT->GetUserTypeList(i)->GetName() == "$parameter"sv) {
						continue;
					}
					result += styleUT->GetUserTypeList(i)->GetName();
					wiz::load_data::UserType temp = *(styleUT->GetUserTypeList(i));

					result += "@\'" + wiz::load_data::LoadData::ToBool4(nullptr, *mainUT, temp, executeData).ToString();
					result += "\'";
				}

				option = option + OptionFrom(result, mainUT, styleMap);
			}
			else {
				std::cout << "wrong option" << ENTER;
			}

			start = end_;
		}

		if (option.type.empty()) {
			option.type.push_back(Option::Type_::ANY);
		}
		return option;
	}

	inline bool OptionDoA(Option option, const std::string& str) 
	{
		if (wiz::String::startsWith(option.prefix, "$"sv)) {
			option.prefix = option.prefix.substr(1);
			if (option.prefix == "_"sv) {
				option.prefix = std::string();
			}
		}

		if (option.prefix.empty() == false &&
			option.prefix == str) {
			return true;
		}
		if (option.prefix.empty() && str.empty()) {
			return true;
		}

		long long count = option.type.size();
		auto type_list = option.type;

		while (!type_list.empty()) {
			switch (type_list.back()) {
			case Option::Type_::ANY:
				// pass
				break;
			case Option::Type_::INT:
				if (wiz::load_data::Utility::IsIntegerInJson(str)) {
					if (option.use_minimum) {
						wiz::DataType x(str.data());
						if (option.minimum.ToInt() <= x.ToInt()) {
							//
						}
						else {
							std::cout << str << " is small" << ENTER;
							count--;
						}
					}
					if (option.use_maximum) {
						wiz::DataType x(str.data());
						if (option.maximum.ToInt() >= x.ToInt()) {
							//
						}
						else {
							std::cout << str << " is big" << ENTER;
							count--;
						}
					}
				}
				else {
					std::cout << str << " is not integer" << ENTER;
					count--;
				}
				break;
			case Option::Type_::FLOAT:
				if (wiz::load_data::Utility::IsFloatInJson(str)) {
					if (option.use_minimum) {
						wiz::DataType x(str.data());
						if (option.minimum.ToFloat() <= x.ToFloat()) {
							//
						}
						else {
							std::cout << str << " is small" << ENTER;
							count--;
						}
					}
					if (option.use_maximum) {
						wiz::DataType x(str.data());
						if (option.maximum.ToFloat() >= x.ToFloat()) {
							//
						}
						else {
							std::cout << str << " is big" << ENTER;
							count--;
						}
					}
				}
				else {
					std::cout << str << " is not float" << ENTER;
					count--;
				}
				break;

			case Option::Type_::QUOTED_STRING:
				 // quoted string.
				if (str.size() >= 2 && str[0] == str.back() && str.back() == '\"') { // cf '\''
					//
				}
				else {
					std::cout << str << " is not quoted string" << ENTER;
					count--;
				}
				
				break;
			case Option::Type_::STRING:
				break;
			}
			type_list.pop_back();
		}

		if (count <= 0) {
			return false;
		}

		bool success = false;
		// chk regex
		if (!option.regex.empty()) {
			std::regex rgx(option.regex);

			if (std::regex_match(str, rgx)) {
				success = true;
			}
			if (!success) {
				return false;
			}
		}

		return true;
	}


	class ValidationInfo {
	public:
		const wiz::load_data::UserType* nowTextUT;
		const wiz::load_data::UserType* nowSchemaUT;

		size_t index;
		size_t itCount;
		size_t utCount;

		std::vector<int> visited; // for nowTextUT. // for it
		std::vector<int> visited2; // for ut

		bool isOrdered = false; // object -> false?, array -> true?, and mixed?

	public:
		ValidationInfo(const wiz::load_data::UserType* nowTextUT, const wiz::load_data::UserType* nowSchemaUT)
			: nowTextUT(nowTextUT), nowSchemaUT(nowSchemaUT), index(0), itCount(0), utCount(0)
		{

		}
	};


	bool Validate(const wiz::load_data::UserType* textUT, const wiz::load_data::UserType* schemaUT, wiz::load_data::UserType* eventUT, 
			const std::map<std::string, wiz::load_data::UserType*>& styleMap, const std::map<std::string, std::set<std::string>>& enumMap) {
		
		std::map<std::string, std::set<std::string>> ids;
		std::map<std::string, std::string> needMap;

		bool print_off = true;
		std::queue<ValidationInfo> que;

		que.push(ValidationInfo(textUT, schemaUT));
		// visited..
		que.front().visited = std::vector<int>(textUT->GetItemListSize(), 0);
		que.front().visited2 = std::vector<int>(textUT->GetUserTypeListSize(), 0);


		while (!que.empty()) {
			auto iter = que.front();
			que.pop();

			// schemaUT..
			int fail = 0;

			size_t i = iter.index;
			if (i < iter.nowSchemaUT->GetIListSize()) {
				if (iter.nowSchemaUT->IsItemList(i)) {
					if (iter.nowSchemaUT->GetItemList(iter.itCount).GetName() == "$id_clear"sv) {
						ids.erase(iter.nowSchemaUT->GetItemList(iter.itCount).Get());
						iter.index++;
						iter.itCount++;

						que.push(iter);
						continue;
					}
					else if (iter.nowSchemaUT->GetItemList(iter.itCount).GetName() == "$print"sv) {
						if (iter.nowSchemaUT->GetItemList(iter.itCount).Get() == "on") {
							print_off = false;
						}
						else {
							print_off = true;
						}

						iter.index++;
						iter.itCount++;

						que.push(iter);
						continue;
					}

					// key = val or val
					Option opt_key = OptionFrom(iter.nowSchemaUT->GetItemList(iter.itCount).GetName(), eventUT, styleMap);
					Option opt_data = OptionFrom(iter.nowSchemaUT->GetItemList(iter.itCount).Get(), eventUT, styleMap);

					std::vector<long long> text_idx;

					if (wiz::String::startsWith(opt_key.prefix, "$"sv)) {
						if (opt_key.prefix == "$"sv) {
							for (int k = 0; k < iter.nowTextUT->GetItemListSize(); ++k) {
								text_idx.push_back(k);
							}
						}
						else {
							if (opt_key.prefix == "$_"sv) {
								text_idx = iter.nowTextUT->GetItemIdx("");
							}
							else {
								text_idx = iter.nowTextUT->GetItemIdx(opt_key.prefix.substr(1));
							}
						}
					}
					else {
						text_idx = iter.nowTextUT->GetItemIdx(opt_key.prefix);
					}


					// chk optional, (required)
					if (text_idx.empty() && opt_key.required == Option::Required_::OPTIONAL_) {
						iter.index++;
						iter.itCount++;

						que.push(iter);

						continue;
					}
					else if (text_idx.empty()) {
						if (!print_off) {
							wiz::Out << "it is not optional..\n" << opt_key.prefix << "\n";
						}

						return false;
					}

					// DoA - chk minimum, maximum, type, regex,
					for (int j = 0; j < text_idx.size(); ++j) {
						int success = 0;
						if (OptionDoA(opt_key, iter.nowTextUT->GetItemList(text_idx[j]).GetName())) {
							success++;
						}
						else {
							if (!print_off) {
								wiz::Out << "key is not valid\n" << opt_key.prefix << "\n";
							}
						}

						if (OptionDoA(opt_data, iter.nowTextUT->GetItemList(text_idx[j]).Get())) {
							success++;
						}
						else {
							if (!print_off) {
								wiz::Out << "data is not valid\n";
							}
						}

						if (success < 2) {
							return false;
						}
					}
					
					// chk enum.
					{
						if (!opt_key.enum_ids.empty()) {
							std::vector<int> chk(text_idx.size(), 0);

							for (int k = 0; k < opt_key.enum_ids.size(); ++k) {
								auto ids = enumMap.find(opt_key.enum_ids[k]);
								
								for (int j = 0; j < text_idx.size(); ++j) {
									if (ids->second.end() != ids->second.find(iter.nowTextUT->GetItemList(text_idx[j]).GetName())) {
										chk[j]++;
									}
								}
							}

							for (int j = 0; j < chk.size(); ++j) {
								if (0 == chk[j]) {
									if (!print_off) {
										wiz::Out << "key is not valid enum.\n";
									}
									return false;
								}
							}
						}

						if (!opt_data.enum_ids.empty()) {
							std::vector<int> chk(text_idx.size(), 0);

							for (int k = 0; k < opt_data.enum_ids.size(); ++k) {
								auto ids = enumMap.find(opt_data.enum_ids[k]);

								for (int j = 0; j < text_idx.size(); ++j) {
									if (ids->second.end() != ids->second.find(iter.nowTextUT->GetItemList(text_idx[j]).Get())) {
										chk[j]++;
									}
								}
							}

							for (int j = 0; j < chk.size(); ++j) {
								if (0 == chk[j]) {
									if (!print_off) {
										wiz::Out << "data is not valid enum.\n";
									}
									return false;
								}
							}
						}
					}

					// id
					{
						if (!opt_key.ids.empty()) {
							for (int k = 0; k < opt_key.ids.size(); ++k) {
								for (int j = 0; j < text_idx.size(); ++j) {
									if (ids.find(opt_key.ids[k]) == ids.end()) {
										ids.insert(std::make_pair(opt_key.ids[k], std::set<std::string>()));
									}
									auto x = ids.find(opt_key.ids[k]);
									if (x->second.end() == x->second.find(iter.nowTextUT->GetItemList(text_idx[j]).GetName())) {
										x->second.insert(iter.nowTextUT->GetItemList(text_idx[j]).GetName());
									}
									else {
										if (!print_off) {
											wiz::Out << "id is not valid.\n";
										}
										return false;
									}
								}
							}
						}
						if (!opt_data.ids.empty()) {
							for (int k = 0; k < opt_data.ids.size(); ++k) {
								for (int j = 0; j < text_idx.size(); ++j) {
									if (ids.find(opt_data.ids[k]) == ids.end()) {
										ids.insert(std::make_pair(opt_data.ids[k], std::set<std::string>()));
									}
									auto x = ids.find(opt_data.ids[k]);
									if (x->second.end() == x->second.find(iter.nowTextUT->GetItemList(text_idx[j]).Get())) {
										x->second.insert(iter.nowTextUT->GetItemList(text_idx[j]).Get());
									}
									else {
										if (!print_off) {
											wiz::Out << "id is not valid.\n";
										}
										return false;
									}
								}
							}

						}
					}

					// need?					
					for (size_t j = 0; j < opt_key.needs.size(); ++j) {
						std::string old = iter.nowSchemaUT->GetItemList(iter.itCount).GetName();
						old = old.substr(1, old.size() - 2);

						if (auto _x = needMap.find(opt_key.needs[j]); _x != needMap.end()) {
							while (_x != needMap.end()) {
								if (_x->second == old) {
									if (!print_off) {
										wiz::Out << "need error\n";
									}
									return false;
								}
								_x = needMap.find(_x->second);
							}
						}

						needMap.insert({ old, opt_key.needs[j] });


						if (iter.nowSchemaUT->GetParent() != nullptr) {
							if ((!iter.nowSchemaUT->GetItem("\"" + opt_key.needs[j] + "\"").empty()) ||
								(!iter.nowSchemaUT->GetUserTypeItem("\"" + opt_key.needs[j] + "\"").empty())) {
								//
							}
							else {
								if (!print_off) {
									wiz::Out << "err) need " << ("\"" + opt_key.needs[j] + "\"") << "\n";
								}
								return false;
							}
						}
					}

					// Event..
					{
						for (int t = 0; t < text_idx.size(); ++t) {
							for (auto& x : opt_key.event_ids) {
								auto y = iter.nowTextUT->GetItemList(text_idx[t]);
								wiz::load_data::UserType* mainUT = eventUT;
								std::vector<std::string> vec = wiz::tokenize(x, '@');
								std::string event_name = vec[0];
								std::string argument;
								if (vec.size() > 1) {
									argument = vec[1].substr(1, vec[1].size() - 2);
									argument = wiz::load_data::LoadData::ToBool4(nullptr, *mainUT, argument, wiz::ExecuteData()).ToString();
								}

								// for var // chk no start with __, no end with __ ?

								std::string func = "Event = { id = __";

								func += event_name;
								func += "__ $call = { id = ";
								func += event_name;
								func += " name = ";
								func += y.GetName().empty()? "$NO_NAME" : y.GetName(); // todo - empty Ã³¸®.
								func += " value = ";
								func += y.Get(0);
								func += " is_usertype = FALSE ";

								func += " select = NAME ";
								func += " input = ";
								func += y.GetName().empty() ? "$NO_NAME" : y.GetName();
								func += " ";
								func += argument;
								func += " } } ";

								wiz::load_data::LoadData::AddData(*mainUT, "/./",
									func,
									wiz::ExecuteData());
								wiz::Option option;
								if ("TRUE"sv == clauText.execute_module("Main = { $call = { id = __" + event_name + "__ } }", mainUT, wiz::ExecuteData(), option, 1)) {
									mainUT->RemoveUserTypeList(mainUT->GetUserTypeListSize() - 1);
								}
								else {
									mainUT->RemoveUserTypeList(mainUT->GetUserTypeListSize() - 1);

									std::cout << "clauText is not valid1" << ENTER;
									return false;
								}
							}
							for (auto& x : opt_data.event_ids) {
								auto y = iter.nowTextUT->GetItemList(text_idx[t]);

								wiz::load_data::UserType* mainUT = eventUT;
								std::vector<std::string> vec = wiz::tokenize(x, '@');
								std::string event_name = vec[0];
								std::string argument;
								if (vec.size() > 1) {
									argument = vec[1].substr(1, vec[1].size() - 2);
									argument = wiz::load_data::LoadData::ToBool4(nullptr, *mainUT, argument, wiz::ExecuteData()).ToString();
								}

								// for val

								std::string func = "Event = { id = __";

								func += event_name;
								func += "__ $call = { id = ";
								func += event_name;
								func += " name = ";
								func += y.GetName().empty() ? "$NO_NAME" : y.GetName();
								func += " value = ";
								func += y.Get(0);
								func += " is_usertype = FALSE ";
								func += " select = VALUE ";
								func += " input = ";
								func += y.Get();
								func += " ";
								func += argument;
								func += " } } ";

								wiz::load_data::LoadData::AddData(*mainUT, "/./",
									func,
									wiz::ExecuteData());
								wiz::Option option;
								if ("TRUE"sv == clauText.execute_module("Main = { $call = { id = __" + event_name + "__ } }", mainUT, wiz::ExecuteData(), option, 1)) {
									mainUT->RemoveUserTypeList(mainUT->GetUserTypeListSize() - 1);
								}
								else {
									mainUT->RemoveUserTypeList(mainUT->GetUserTypeListSize() - 1);

									if (!print_off) {
										wiz::Out << "clauText is not valid2" << ENTER;
									}
									return false;
								}
							}
						}
					}


					iter.index++;
					iter.itCount++;

					for (int j = 0; j < text_idx.size(); ++j) {
						iter.visited[text_idx[j]]++;
						if (iter.visited[text_idx[j]] > 1) {
							if (!print_off) {
								wiz::Out << "dupllicated..\n";
							}
							return false;
						}
					}


					que.push(iter);
				}
				else { // usertype..
					// key = { } or  { }
					Option opt_key = OptionFrom(iter.nowSchemaUT->GetUserTypeList(iter.utCount)->GetName(), eventUT, styleMap);

					std::vector<long long> text_idx;

					if (wiz::String::startsWith(opt_key.prefix, "$"sv)) {
						if (opt_key.prefix == "$"sv) {
							for (int k = 0; k < iter.nowTextUT->GetUserTypeListSize(); ++k) {
								text_idx.push_back(k);
							}
						}
						else {
							if (opt_key.prefix == "$_"sv) {
								text_idx = iter.nowTextUT->GetUserTypeItemIdx("");
							}
							else {
								text_idx = iter.nowTextUT->GetUserTypeItemIdx(opt_key.prefix.substr(1));
							}
						}
					}
					else {
						text_idx = iter.nowTextUT->GetUserTypeItemIdx(opt_key.prefix);
					}


					// chk optional, (required)
					if (text_idx.empty() && opt_key.required == Option::Required_::OPTIONAL_) {
						iter.index++;
						iter.utCount++;

						que.push(iter);

						continue;
					}
					else if (text_idx.empty()) {
						if (!print_off) {
							wiz::Out << "it not optional..\n";
						}

						return false;
					}

					// DoA - chk minimum, maximum, type, regex,
					for (int j = 0; j < text_idx.size(); ++j) {
						int success = 0;
						if (OptionDoA(opt_key, iter.nowTextUT->GetUserTypeList(text_idx[j])->GetName())) {
							success++;
						}
						else {
							if (!print_off) {
								wiz::Out << "key is not valid\n" << opt_key.prefix <<"\n";
							}
						}
						
						if (success < 1) {
							return false;
						}
					}

					// chk enum.
					{
						if (!opt_key.enum_ids.empty()) {
							std::vector<int> chk(text_idx.size(), 0);

							for (int k = 0; k < opt_key.enum_ids.size(); ++k) {
								auto ids = enumMap.find(opt_key.enum_ids[k]);

								for (int j = 0; j < text_idx.size(); ++j) {
									if (ids->second.end() != ids->second.find(iter.nowTextUT->GetUserTypeList(text_idx[j])->GetName())) {
										chk[j]++;
									}
								}
							}

							for (int j = 0; j < chk.size(); ++j) {
								if (0 == chk[j]) {
									if (!print_off) {
										wiz::Out << "key is not valid enum.\n";
									}
									return false;
								}
							}
						}
					}

					// id //chk?
					/*{
						if (!opt_key.ids.empty()) {
							for (int k = 0; k < opt_key.ids.size(); ++k) {
								for (int j = 0; j < text_idx.size(); ++j) {
									if (ids.find(opt_key.ids[k]) == ids.end()) {
										ids.insert(std::make_pair(opt_key.ids[k], std::set<std::string>()));
									}
									auto x = ids.find(opt_key.ids[k]);
									if (x->second.end() == x->second.find(iter.nowTextUT->GetUserTypeList(text_idx[j])->GetName())) {
										x->second.insert(iter.nowTextUT->GetUserTypeList(text_idx[j])->GetName());
									}
									else {
										if (!print_off) {
											wiz::Out << "id is not valid.\n";
										}
										return false;
									}
								}
							}
						}
					}*/

					// need?					
					for (size_t j = 0; j < opt_key.needs.size(); ++j) {
						std::string old = iter.nowSchemaUT->GetUserTypeList(iter.utCount)->GetName();
						old = old.substr(1, old.size() - 2);

						if (auto _x = needMap.find(opt_key.needs[j]); _x != needMap.end()) {
							while (_x != needMap.end()) {
								if (_x->second == old) {
									if (!print_off) {
										wiz::Out << "need error\n";
									}
									return false;
								}
								_x = needMap.find(_x->second);
							}
						}

						needMap.insert({ old, opt_key.needs[j] });


						if (iter.nowSchemaUT->GetParent() != nullptr) {
							if ((!iter.nowSchemaUT->GetItem("\"" + opt_key.needs[j] + "\"").empty()) ||
								(!iter.nowSchemaUT->GetUserTypeItem("\"" + opt_key.needs[j] + "\"").empty())) {
								//
							}
							else {
								if (!print_off) {
									wiz::Out << "err) need " << ("\"" + opt_key.needs[j] + "\"") << "\n";
								}
								return false;
							}
						}
					}

					// Event..
					{
						for (int t = 0; t < text_idx.size(); ++t) {
							for (auto& x : opt_key.event_ids) {
								auto y = iter.nowTextUT->GetUserTypeList(text_idx[t]);
								wiz::load_data::UserType* mainUT = eventUT;
								std::vector<std::string> vec = wiz::tokenize(x, '@');
								std::string event_name = vec[0];
								std::string argument;
								if (vec.size() > 1) {
									argument = vec[1].substr(1, vec[1].size() - 2);
									argument = wiz::load_data::LoadData::ToBool4(nullptr, *mainUT, argument, wiz::ExecuteData()).ToString();
								}

								// for var // chk no start with __, no end with __ ?

								std::string func = "Event = { id = __";

								func += event_name;
								func += "__ $call = { id = ";
								func += event_name;
								func += " name = ";
								func += y->GetName().empty() ? "$NO_NAME" : y->GetName();
								
								func += " is_usertype = TRUE ";

								func += " select = NAME ";
								func += " input = ";
								func += y->GetName().empty() ? "$NO_NAME" : y->GetName();
								func += " ";
								func += argument;
								func += " } } ";

								wiz::load_data::LoadData::AddData(*mainUT, "/./",
									func,
									wiz::ExecuteData());

								wiz::Option opt;
								if ("TRUE"sv == clauText.execute_module("Main = { $call = { id = __" + event_name + "__ } }", mainUT, wiz::ExecuteData(), opt, 1)) {
									mainUT->RemoveUserTypeList(mainUT->GetUserTypeListSize() - 1);
								}
								else {
									mainUT->RemoveUserTypeList(mainUT->GetUserTypeListSize() - 1);

									std::cout << "clauText is not valid1" << ENTER;
									return false;
								}
							}
						}
					}

					for (int j = 0; j < text_idx.size(); ++j) {
						iter.visited2[text_idx[j]]++;
						if (iter.visited2[text_idx[j]] > 1) {
							if (!print_off) {
								wiz::Out << "dupllicated..\n";
							}
							return false;
						}
					}

					iter.index++;
					iter.utCount++;

					// push to stack.
					// chk now text ut -> child

					for (int j = 0; j < text_idx.size(); ++j) {
						que.push(Lint::ValidationInfo(iter.nowTextUT->GetUserTypeList(text_idx[j]), iter.nowSchemaUT->GetUserTypeList(iter.utCount - 1)));
						que.back().visited = std::vector<int>(iter.nowTextUT->GetUserTypeList(text_idx[j])->GetItemListSize(), 0);
						que.back().visited2 = std::vector<int>(iter.nowTextUT->GetUserTypeList(text_idx[j])->GetUserTypeListSize(), 0);
					}
					que.push(iter);
				}
			}
			else { // i == iter.nowSchemaUT->GetIListSize()
				// chk visited and visited2
				for (int i = 0; i < iter.visited.size(); ++i) {
					if (iter.visited[i] == 0) {
						if (!print_off) {
							wiz::Out << "no visit1\n";
						}
						return false;
					}
				}
				for (int i = 0; i < iter.visited2.size(); ++i) {
					if (iter.visited2[i] == 0) {
						if (!print_off) {
							wiz::Out << "no visit2\n";
						}
						return false;
					}
				}
			}
		}


		return true;
	}
}


int main(int argc, char* argv[]) // program_name text_file schema_file
{
	if (argc != 3) {
		std::cout << "text_file schema_file\n";
		return -1;
	}

	try {

		std::string text_file = argv[1];
		std::string schema_file = argv[2];

		wiz::load_data::UserType* textUT = new wiz::load_data::UserType("Text");
		wiz::load_data::UserType schemaUT;

		if (!wiz::load_data::LoadData::LoadDataFromFile(text_file, *textUT)) {
			std::cout << "loading text file is failed\n";
			return -2;
		}
		if (!wiz::load_data::LoadData::LoadDataFromFile3(schema_file, schemaUT, 0, 0)) {
			std::cout << "loading schema file is failed\n";
			return -3;
		}
		
		schemaUT.LinkUserType(textUT);

		// call __init__ Event...
		{
			wiz::ClauText clautext;

			wiz::Option opt;
			clautext.execute_module(" Main = { $call = { id = __init__ } } ", &schemaUT, wiz::ExecuteData(), opt, 1);
		}

		// chk schemaUT has  Text = { }
		auto x = schemaUT.GetUserTypeItem("Schema");

		if (x.empty()) {
			std::cout << "schema is not valid\n";
			return -4;
		}

		auto styleList = schemaUT.GetUserTypeItem("Style");
		std::map<std::string, wiz::load_data::UserType*> style_map;

		for (auto x : styleList) {
			style_map.insert(std::make_pair(x->GetItem("id")[0].Get(), x));
		}

		auto enumList = schemaUT.GetUserTypeItem("Enum");
		std::map<std::string, std::set<std::string>> enum_map;

		for (auto x : enumList) {
			std::set<std::string> _list;

			for (int i = 0; i < x->GetItemListSize(); ++i) {
				if (x->GetItemList(i).GetName().empty()) {
					_list.insert(x->GetItemList(i).Get());
				}
			}

			enum_map.insert(std::make_pair(x->GetItem("id")[0].Get(), _list));
		}

		if (!Lint::Validate(textUT, x[0], &schemaUT, style_map, enum_map)) {
			std::cout << "text file is not valid with schema\n";
			return -5;
		}
		else {
			std::cout << "success\n";
		}


		// call __end__ Event...
		{
			wiz::ClauText clautext;

			wiz::Option opt;
			clautext.execute_module(" Main = { $call = { id = __end__ } } ", &schemaUT, wiz::ExecuteData(), opt, 1);
		}

	}
	catch (...) {
		std::cout << "internal error\n";
		return -100;
	}

	return 0;
}

