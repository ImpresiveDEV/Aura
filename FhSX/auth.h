#include <Windows.h>
#include <iostream>
#include <vector>
#include <fstream>

struct channel_struct
{
	std::string author;
	std::string message;
	std::string timestamp;
};

namespace KeyAuth {
	class api {
	public:

		std::string name, ownerid, secret, version, url;

		api(std::string name, std::string ownerid, std::string secret, std::string version, std::string url) : name(name), ownerid(ownerid), secret(secret), version(version), url(url) {}

		void init();

		class subscriptions_class {
		public:
			std::string name;
			std::string expiry;
		};

		class data_class {
		public:
			std::string numUsers;
			std::string numOnlineUsers;
			std::string numKeys;
			std::string version;
			std::string customerPanelLink;

			std::string username;
			std::string ip;
			std::string hwid;
			std::string createdate;
			std::string lastlogin;

			std::vector<subscriptions_class> subscriptions;

			std::vector<channel_struct> channeldata;
			bool success;
			std::string message;
		};
		data_class data;

	private:
		std::string sessionid, enckey;
	};
}