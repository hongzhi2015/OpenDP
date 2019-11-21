/////////////////////////////////////////////////////////////////////////////
// Authors: SangGi Do(sanggido@unist.ac.kr), Mingyu Woo(mwoo@eng.ucsd.edu)
//          (respective Ph.D. advisors: Seokhyeong Kang, Andrew B. Kahng)
//
//          Original parsing structure was made by Myung-Chul Kim (IBM).
//
// BSD 3-Clause License
//
// Copyright (c) 2018, SangGi Do and Mingyu Woo
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// * Neither the name of the copyright holder nor the names of its
//   contributors may be used to endorse or promote products derived from
//   this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
///////////////////////////////////////////////////////////////////////////////

#include "circuit.h"

using opendp::circuit;
using opendp::cell;
using opendp::row;
using opendp::pixel;
using opendp::rect;
using opendp::macro;
using opendp::site;
using opendp::group;
using opendp::density_bin;

using std::max;
using std::min;
using std::pair;
using std::cout;
using std::cerr;
using std::endl;
using std::istream;
using std::ifstream;
using std::ofstream;
using std::vector;
using std::make_pair;
using std::to_string;
using std::string;
using std::fixed;
using std::numeric_limits;

/* generic helper functions */
bool opendp::is_special_char(char c) {
  static const char specialChars[] = {'(', ')', ',', ':', ';', '/',  '#',
                                      '[', ']', '{', '}', '*', '\"', '\\'};

  for(unsigned i = 0; i < sizeof(specialChars); ++i) {
    if(c == specialChars[i]) return true;
  }

  return false;
}

bool opendp::read_line_as_tokens(istream &is, vector< string > &tokens) {
  tokens.clear();

  string line;
  getline(is, line);

  while(is && tokens.empty()) {
    string token = "";
    for(unsigned i = 0; i < line.size(); ++i) {
      char currChar = line[i];
      if(isspace(currChar) || is_special_char(currChar)) {
        if(!token.empty()) {
          // Add the current token to the list of tokens
          tokens.push_back(token);
          token.clear();
        }
        // else if the token is empty, simply skip the whitespace or special
        // char
      }
      else {
        // Add the char to the current token
        token.push_back(currChar);
      }
    }

    if(!token.empty()) tokens.push_back(token);

    if(tokens.empty())
      // Previous line read was empty. Read the next one.
      getline(is, line);
  }

  return !tokens.empty();
}

void opendp::get_next_token(ifstream &is, string &token, const char *beginComment) {
  do {
    is >> token;
    if(!strcmp(token.substr(0, strlen(beginComment)).c_str(), beginComment)) {
      getline(is, token);
      token = "";
    }
  } while(!is.eof() && (token.empty() || isspace(token[0])));
}

void opendp::get_next_n_tokens(ifstream &is, vector< string > &tokens,
                       const unsigned numTokens, const char *beginComment) {
  tokens.clear();
  string token;
  unsigned count = 0;
  do {
    is >> token;
    if(!strcmp(token.substr(0, strlen(beginComment)).c_str(), beginComment)) {
      getline(is, token);
      token = "";
    }
    if(!token.empty() && !isspace(token[0])) {
      tokens.push_back(token);
      ++count;
    }
  } while(!is.eof() && count < numTokens);
}

void cell::print() {
  cout << "|=== BEGIN CELL ===|" << endl;
  cout << "name:               " << name << endl;
  cout << "type:               " << type << endl;
  cout << "orient:             " << cellorient << endl;
  cout << "isFixed?            " << (isFixed ? "true" : "false") << endl;
  for(OPENDP_HASH_MAP< string, unsigned >::iterator it = ports.begin();
      it != ports.end(); it++)
    cout << "port: " << (*it).first << " - " << (*it).second << endl;
  cout << "(init_x,  init_y):  " << init_x_coord << ", " << init_y_coord
       << endl;
  cout << "(x_coord,y_coord):  " << x_coord << ", " << y_coord << endl;
  cout << "[width,height]:      " << width << ", " << height << endl;
  cout << "|===  END  CELL ===|" << endl;
}

void row::print() {
  cout << "|=== BEGIN ROW ===|" << endl;
  cout << "name:              " << name << endl;
  cout << "site:              " << site << endl;
  cout << "(origX,origY):     " << origX << ", " << origY << endl;
  cout << "(stepX,stepY):     " << stepX << ", " << stepY << endl;
  cout << "numSites:          " << numSites << endl;
  cout << "orientation:       " << siteorient << endl;
  cout << "|===  END  ROW ===|" << endl;
}

void site::print() {
  cout << "|=== BEGIN SITE ===|" << endl;
  cout << "name:               " << name << endl;
  cout << "width:              " << width << endl;
  cout << "height:             " << height << endl;
  cout << "type:               " << type << endl;
  for(vector< string >::iterator it = symmetries.begin();
      it != symmetries.end(); ++it)
    cout << "symmetries:         " << *it << endl;
  cout << "|===  END  SITE ===|" << endl;
}


void density_bin::print() {
  cout << "|=== BEGIN DENSITY_BIN ===|" << endl;
  cout << " area :        " << area << endl;
  cout << " m_util :      " << m_util << endl;
  cout << " f_util :      " << f_util << endl;
  cout << " free_space :  " << free_space << endl;
  cout << " overflow :    " << overflow << endl;
  cout << " density limit:" << density_limit << endl;
  cout << "|===  END  DENSITY_BIN ===|" << endl;
}
