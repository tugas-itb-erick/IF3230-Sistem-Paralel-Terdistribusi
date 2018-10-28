#include <server.hpp>
#include <sstream>
#include <iostream>
namespace raft {
	Server::Server(){
		cluster_size = 0;
		server_index = 0;
	    last_applied = 0;
	    commit_index = 0;
	    data = 0;
	    time_to_timeout = 5;
	    voted_for = -1;
	    current_term = 1;
	    server_index = 0;
	    cluster_size = 0;
	    state = State::FOLLOWER;
	}

	Server::Server(int cluster_size_, int server_index_, Sender sender_) : next_index(cluster_size_+1), match_index(cluster_size_+1), sender(sender_), vote_granted(cluster_size_+1){
		cluster_size = 0;
		server_index = 0;
	    last_applied = 0;
	    commit_index = 0;
	    data = 0;
	    time_to_timeout = 5;
	    voted_for = -1;
	    current_term = 1;
		cluster_size = cluster_size_;
		server_index = server_index_;
		state = State::FOLLOWER;
	}

	void Server::Crash(){
		// reset state
		voted_for = -1;
		time_to_timeout = 5;
		state = State::FOLLOWER;
	}

	void Server::SetTimeout(int ttt){
		time_to_timeout = ttt;
	}

	void Server::AdjustCommitIndex(){
		int l = commit_index;
		int r = (int)logs.size() - 1;

		while (l < r) {
			int mid = (l+r)>>1;
			if (logs[mid].term == current_term) 
				r = mid;
			else 
				l = mid + 1;
		}

		if (l < (int)logs.size() && logs[l].term != current_term) 
			return;
		
		r = (int)logs.size() - 1;
		int count = 0;
		while (l <= r) {
			int mid = (l + r + 1) >> 1;
			count = 0;
			
			for (int i=1; i<=cluster_size; i++) {
				if (i == server_index || match_index[i] > mid) 
					++count;
			}

			if (l < r) {
				if (count >= (cluster_size/2 + 1)) 
					l = mid;
				else 
					r = mid - 1;
			} 
			else {
				--r;
			}
		}

		if (l < (int)logs.size() && count >= (cluster_size/2 + 1)) {
			commit_index = l + 1;
		}
	}

	void Server::Timestep(){
		time_to_timeout--;
		if (time_to_timeout <= 0) {
			if (state == State::LEADER) {
				time_to_timeout = 3;

				// Send AppendEntriesRPC to others
				AppendEntriesRPC aerpc = {
					current_term,
					server_index, 
					0, 
					0, 
					std::vector<Log>(1), 
					commit_index
				};
				aerpc.logs[0].term = current_term;
				for (int i=1; i<=cluster_size; i++) {
					if (i != server_index) {
						aerpc.prev_log_index = next_index[i] - 2;
						if (aerpc.prev_log_index >= 0){ 
							aerpc.prev_log_term = logs[aerpc.prev_log_index].term;
						}
						else {
							aerpc.prev_log_term = current_term - 1;
						}

						if (next_index[i] - 1 < logs.size()) {
							aerpc.logs[0] = logs[next_index[i] - 1];
						} else {
							aerpc.logs[0].payload = 0;
							aerpc.logs[0].operation = Operation::NOTHING;
						}
						sender.Send(i, aerpc);
					}
				}
			} else {
				time_to_timeout = 5;
				state = State::CANDIDATE;
				current_term++;

				// vote itself
				voted_for = server_index;
				for (int i=1; i<=cluster_size; i++) {
					vote_granted[i] = false;
				}
				vote_granted[server_index] = true;
				vote_count = 1;

				// Send RequestVoteRPC to others
				RequestVoteRPC rvrpc = {
					current_term, 
					server_index, 
					-1, 
					current_term - 1
				};
				if (!logs.empty()) {
					rvrpc.last_log_index = (int)(logs.size() - 1); 	
					rvrpc.last_log_term = logs.back().term;
				}
				for (int i=1; i<=cluster_size; i++) {
					if (i != server_index) {
						sender.Send(i, rvrpc);
					}
				}
			}
		}

		ApplyLog();
	}

	void Server::Receive(AppendEntriesRPC rpc){
  		AppendEntriesReply reply;
  		reply.from_id = server_index;
  		reply.request = rpc;
  		reply.term = current_term;
  		reply.success = true;
  		if (rpc.term < reply.term) {
  			reply.success = false;
  		} else {
  			if (state == State::CANDIDATE && rpc.term <= reply.term) {
  				state = State::FOLLOWER;
  			}
  			if (rpc.term > reply.term) {
  				current_term = rpc.term;
  				voted_for = -1;
  				state = State::FOLLOWER;
  			}
  			SetTimeout(5);
  			if (rpc.prev_log_index >= (int)logs.size() || (rpc.prev_log_index >= 0 && rpc.prev_log_term != logs[rpc.prev_log_index].term)) {
  				reply.success = false;
  			} else {
				if (rpc.logs[0].operation != Operation::NOTHING) {
					if (rpc.prev_log_index + 1 < (int)logs.size() && logs[rpc.prev_log_index+1].term != rpc.logs[0].term) {
						logs.resize(rpc.prev_log_index + 1);
					}
					if ((int)logs.size() == rpc.prev_log_index + 1) {
						logs.push_back(rpc.logs[0]);
					}
				}
				if (rpc.leader_commit_index > commit_index) {
					if (rpc.leader_commit_index < rpc.prev_log_index + 1) {
						commit_index = rpc.leader_commit_index;
					} else {
						commit_index = rpc.prev_log_index + 1;
					}
				}
			}

  		}
  		sender.Send(rpc.leader_id, reply);

	}

	void Server::Receive(AppendEntriesReply reply){
		if (reply.term > current_term) {
			state = State::FOLLOWER;
			voted_for = -1;
			time_to_timeout = 5;
			current_term = reply.term;
		}

		if (reply.success) {
			if (reply.request.logs[0].operation != Operation::NOTHING) {
				++next_index[reply.from_id];
				match_index[reply.from_id] = next_index[reply.from_id] - 1;
			}
		} else {
			--next_index[reply.from_id];
		}

		AdjustCommitIndex();
		ApplyLog();  
	}

  void Server::Receive(RequestVoteRPC rpc){
		RequestVoteReply rvreply = {
			server_index, 
			rpc, 
			-1, // term
			false // vote_granted
		};
		if (rpc.term >= current_term) {
			if (rpc.term > current_term) {
				time_to_timeout = 5;
				state = State::FOLLOWER;
  				voted_for = -1;
  				current_term = rpc.term;
			}

			if (voted_for == rpc.candidate_id || voted_for == -1) {
  				if (rpc.last_log_index == -1) {
  					if (logs.empty()) {
  						rvreply.vote_granted = true;
  						voted_for = rpc.candidate_id;
  					}
  				} else if (logs.empty()) {
  					rvreply.vote_granted = true;
  					voted_for = rpc.candidate_id;
  				} else {
  					if ((rpc.last_log_term == logs.back().term && (rpc.last_log_index + 1) >= logs.size()) || (rpc.last_log_term > logs.back().term)) {
  						rvreply.vote_granted = true;
  						voted_for = rpc.candidate_id;
  					}
  				}
  			}
		}
		rvreply.term = current_term;
		if (rvreply.vote_granted) {
  			time_to_timeout = 5;
  		}

		sender.Send(rpc.candidate_id, rvreply); 
	}

	void Server::Receive(RequestVoteReply reply){
		if (reply.term > current_term) {
			state = State::FOLLOWER;
			voted_for = -1;
			time_to_timeout = 5;
			current_term = reply.term;
		}

		vote_granted[reply.from_id] = reply.vote_granted;

		if (reply.vote_granted) {
			vote_count++;
		}

		if (vote_count >= (cluster_size/2 + 1) && state == State::CANDIDATE) {
			state = State::LEADER;
			time_to_timeout = 0;

			for (int i = 0; i < match_index.size(); i++) {
				match_index[i] = 0;
			}

			for (int i = 0; i < next_index.size(); i++) {
				next_index[i] = logs.size() + 1;
			}
		}
	}

  void Server::Receive(Log log){
		// receive client request
  		if( state == State::LEADER ){
  			log.term = current_term;
  			logs.push_back(log);
  		}
  	}

	void Server::ApplyLog(){
		for (int i = last_applied; i < commit_index; i++) {
			if (logs[i].operation == Operation::MULTIPLY) {
				data *= logs[i].payload;
			} else if (logs[i].operation == Operation::ADD) {
				data += logs[i].payload;
			} else if (logs[i].operation == Operation::SUBTRACT) {
				data -= logs[i].payload;
			} else if (logs[i].operation == Operation::REPLACE) {
				data = logs[i].payload;
			}
		}
		last_applied = commit_index;
	}


	std::ostream & operator<<(std::ostream &os, const Server& s){
		std::string state_str = s.GetRoleString();


		std::string log_str = s.GetLogString();

		os << "S" << s.server_index << " "
				  << state_str << " "
				  << "term:" << s.current_term << " " 
				  << "voted_for:" << s.voted_for << " "
				  << "commit_index:" << s.commit_index << " "
				  << "data:" << s.data << " "
				  << "logs:" << log_str;
		return os;
	}

	
	std::string Server::GetLeaderStateString() const {
		std::stringstream ss;

		ss << "L" << server_index << " ";
		ss << "next_index[";
		for( int i = 1; i <= cluster_size; ++ i ){
			if( i == server_index )
				ss << "X ";
			else
				ss << next_index[i] << " ";
		}
		ss << "] ";

		ss << "match_index[";
		for( int i = 1; i <= cluster_size; ++ i ){
			if( i == server_index )
				ss << "X ";
			else
				ss << match_index[i] << " ";
		}
		ss << "]";
		return ss.str();
	}

	std::string Server::GetRoleString() const {
		std::string state_str = "";

		if( state == State::FOLLOWER ) state_str = "follower";
		else if( state == State::LEADER ) state_str = "leader";
		else if( state == State::CANDIDATE ) state_str = "candidate";

		return state_str;
	}

	std::string Server::GetLogString() const {
		std::stringstream log_str; 
		log_str << "T0[0";

		int cterm = 0;
		for( int i = 0; i < logs.size(); ++ i ){
			if( cterm != logs[i].term ){
				log_str << "] T" << logs[i].term << "[";
				cterm = logs[i].term;
			}

			if( logs[i].operation == Operation::MULTIPLY ){
				log_str << "*" << logs[i].payload;
			} else if( logs[i].operation == Operation::ADD ){
				log_str << "+" << logs[i].payload;
			} else if( logs[i].operation == Operation::SUBTRACT ){
				log_str << "-" << logs[i].payload;
			} else if( logs[i].operation == Operation::REPLACE ){
				log_str << "||" << logs[i].payload;
			}
		}
		log_str << "]";

		return log_str.str();
	}

	int Server::GetData() const {
		return data;
	}

	int Server::GetCommitIndex() const {
		return commit_index;
	}
}