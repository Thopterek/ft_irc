/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ndziadzi <ndziadzi@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/24 17:14:45 by sudaniel          #+#    #+#             */
/*   Updated: 2025/07/09 11:09:08 by ndziadzi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_HPP
#define PARSER_HPP

#include <sstream>
#include <vector>
#include <unordered_map>
#include <cctype>
#include "User.hpp"

class   Parser
{
  public:
    using Cmd = void (*)(Client&, int, const std::vector<std::string>&);
    Parser();
    ~Parser() = default;
    Parser(const Parser&) = delete;
    Parser& operator=(const Parser&) = delete;
    static void parseAndDispatch(Client&, int);
  private:
    static std::unordered_map<std::string, Cmd> m_cmds;
    static std::vector<std::string> tokenize(std::string_view);
    static  void    dispatchCommand(Client&, int fd, const std::vector<std::string>&);
};

void    pass(Client&, int, const std::vector<std::string>&);
void    nick(Client&, int, const std::vector<std::string>&);
void    user(Client&, int, const std::vector<std::string>&);
void    privmsg(Client&, int, const std::vector<std::string>&);

#endif
