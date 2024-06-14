/**********************************************************************/
/*           This is the podem test pattern generator for atpg        */
/*                                                                    */
/*           Author: Bing-Chen (Benson) Wu                            */
/*           last update : 01/21/2018                                 */
/**********************************************************************/

#include "atpg.h"

#define CONFLICT 2

/* generates a single pattern for a transition delay fault */
int ATPG::tdfpodem(const fptr fault, int &current_backtracks) {
  bool nieh_speaks = false;
  bool nieh_speaks_details = false;
  string vec;
  int i, ncktwire, ncktin, r, y;
  forward_list<wptr> decision_tree; // design_tree (a LIFO stack)
  wptr wfault;
  int attempt_num = 0;  // counts the number of pattern generated so far for the given fault
  if (nieh_speaks) display_fault(fault);
  forward_list<wptr> decision_tree_for_pattern_1; // design_tree (a LIFO stack)
  int no_of_backtracks_1;
  *fake_fault = *fault;
  fake_fault->fault_type ^= 1;
  vector<string> total_no_compression_patterns;
  vector<string> no_compression_patterns;
  vector<string> temp;
  string st0;
  // int no_compression_attempt_num = 0;  // counts the number of pattern generated so far for the given fault
  vector<int> random_index;

  /* initialize all circuit wires to unknown */
  ncktwire = sort_wlist.size();
  ncktin = cktin.size();
  for (i = 0; i < ncktwire; i++) {
    sort_wlist[i]->value = U;
  }
  no_of_backtracks = 0;
  find_test = false;
  no_test = false;

  mark_propagate_tree(fault->node);

  /* Fig 7 starts here */
  /* set the initial objective, assign the first PI.  Fig 7.P1 */
  switch (set_uniquely_implied_value(fault)) {
    case TRUE: // if a  PI is assigned
      sim();  // Fig 7.3
      wfault = fault_evaluate(fault);
      if (wfault != nullptr) forward_imply(wfault);// propagate fault effect
      if (check_test()) {
        // if (total_attempt_num > 1) {
        //   display_io();
          
        // }
        vec.clear();
        for (wptr w: cktin) {
          vec.push_back(itoc(w->value));
          w->all_assigned = w->is_all_assigned();
        }
        for (i = cktin.size(); i < ncktwire; i++) {
          sort_wlist[i]->value = U;
        }
        for (i = 0; i < cktin.size()-1; i++) {
          sort_wlist[i]->value = ctoi(vec[i+1]); //LOS: find first pattern
        }
        sort_wlist[cktin.size()-1]->value = U;
        // *fake_fault = *fault;
        // fake_fault->fault_type ^= 1;
        if (backward_imply(sort_wlist[fake_fault->to_swlist], !(fake_fault->fault_type)) != CONFLICT) {
            sim();  // Fig 7.3
            if (sort_wlist[fault->to_swlist]->value == (fake_fault->fault_type)) {
              // decision_tree_for_pattern_1.clear();
              for (i = cktin.size(); i < ncktwire; i++) {
                  sort_wlist[i]->value = U;
              }
              for (i = 0; i < cktin.size(); i++) {
                  sort_wlist[i]->value = ctoi(vec[i]); //change back to original first pattern
                  if (sort_wlist[i]->all_assigned) sort_wlist[i]->set_all_assigned();
                  else sort_wlist[i]->remove_all_assigned();
              }
              wpit = nullptr;
              for (wptr w: cktin) {
                w->set_changed();
              }
              sim();
              goto backtrack;
            }
            else if (sort_wlist[fault->to_swlist]->value == !(fake_fault->fault_type)) {
              //Patterns found
              // fprintf(stdout, "T\'");
              no_compression_patterns.clear();
              switch (cktin[0]->value) {
                case 0:
                case D_bar:
                  no_compression_patterns.push_back("0");
                  // fprintf(stdout, "0");
                  break;
                case 1:
                case D:
                  no_compression_patterns.push_back("1");
                  // fprintf(stdout, "1");
                  break;
                case U:
                  no_compression_patterns.push_back("0");
                  no_compression_patterns.push_back("1");
                  // fprintf(stdout, "x");
                  break;
              }
              
              for (i = 1; i < cktin.size(); i++) {
                switch (cktin[i]->value) {
                  case 0:
                  case D_bar:
                    // fprintf(stdout, "0");
                    for (auto& st : no_compression_patterns) {
                      st = st + "0";
                    }
                    break;
                  case 1:
                  case D:
                    // fprintf(stdout, "1");
                    for (auto& st : no_compression_patterns) {
                      st = st + "1";
                    }
                    break;
                  case U:
                    // fprintf(stdout, "x");
                    
                    if (no_compression_patterns.size()+total_no_compression_patterns.size() >= total_attempt_num) {
                      for (auto& st : no_compression_patterns) {
                        st = st + to_string(rand()&1);
                      }
                      break;
                    }
    
                    temp = no_compression_patterns;
                    for (auto& st : no_compression_patterns) {
                      st = st + "0";
                    }
                    for (auto st : temp) {
                      st = st + "1";
                      no_compression_patterns.push_back(st);
                    }
                    break;
                }
              }
              // cout << " " << vec[0];
              // fprintf(stdout, "'\n");
              find_test = true; // if fault effect reaches PO, done. Fig 7.10
              attempt_num++;
              // no_compression_attempt_num += no_compression_patterns.size();
              for (auto st : no_compression_patterns) {
                if (vec[0] == 'x') {
                  st0 = st + " 0";
                  if (no_compression_patterns.size()+total_no_compression_patterns.size() >= total_attempt_num) {
                    st = st + " " + to_string(rand()&1);
                  }
                  else {
                    st = st + " 1";
                    total_no_compression_patterns.push_back(st0);
                  }
                }
                else {
                  st = st + " " + vec[0];
                }
                
                total_no_compression_patterns.push_back(st);
              }
              decision_tree_for_pattern_1.clear();
              for (i = cktin.size(); i < ncktwire; i++) {
                sort_wlist[i]->value = U;
              }
              for (i = 0; i < cktin.size(); i++) {
                sort_wlist[i]->value = ctoi(vec[i]); //change back to original first pattern
                if (sort_wlist[i]->all_assigned) sort_wlist[i]->set_all_assigned();
                else sort_wlist[i]->remove_all_assigned();
              }
              wpit = nullptr;
              
              // goto backtrack;
            }
            else {
              no_of_backtracks_1 = 0;
              while ((no_of_backtracks_1 < backtrack_limit)) {
                /* check if test possible.   Fig. 7.1 */
                if (wpit = test_possible(fake_fault)) {
                  wpit->remove_all_assigned();
                  wpit->set_changed();
                /* insert a new PI into decision_tree */
                  decision_tree_for_pattern_1.push_front(wpit);
                } else { // no test possible using this assignment, backtrack.
                  while (!decision_tree_for_pattern_1.empty() && (wpit == nullptr)) {
                    /* if both 01 already tried, backtrack. Fig.7.7 */
                    if (decision_tree_for_pattern_1.front()->is_all_assigned()) {
                      decision_tree_for_pattern_1.front()->remove_all_assigned();  // clear the ALL_ASSIGNED flag
                      decision_tree_for_pattern_1.front()->value = U; // do not assign 0 or 1
                      decision_tree_for_pattern_1.front()->set_changed(); // this PI has been changed
                    /* remove this PI in decision tree.  see dashed nodes in Fig 6 */
                      decision_tree_for_pattern_1.pop_front();
                    }
                    /* else, flip last decision, flag ALL_ASSIGNED. Fig. 7.8 */
                    else {
                      decision_tree_for_pattern_1.front()->value = decision_tree_for_pattern_1.front()->value ^ 1; // flip last decision
                      decision_tree_for_pattern_1.front()->set_changed(); // this PI has been changed
                      decision_tree_for_pattern_1.front()->set_all_assigned();
                      no_of_backtracks_1++;
                      wpit = decision_tree_for_pattern_1.front();
                    }
                  } // while decision tree && ! wpit
                  
                } // no test possible

            /* this again loop is to generate multiple patterns for a single fault 
            * this part is NOT in the original PODEM paper  */

                if (wpit == nullptr) {
                  decision_tree_for_pattern_1.clear();
                  for (i = cktin.size(); i < ncktwire; i++) {
                      sort_wlist[i]->value = U;
                  }
                  for (i = 0; i < cktin.size(); i++) {
                      sort_wlist[i]->value = ctoi(vec[i]); //change back to original first pattern
                      if (sort_wlist[i]->all_assigned) sort_wlist[i]->set_all_assigned();
                      else sort_wlist[i]->remove_all_assigned();
                  }
                  break;
                  // goto backtrack;
                } //try other pattern 2
                // again:
                else { //if (wpit)
                // cout << wpit->name << endl;
                  sim();
                  if (sort_wlist[fault->to_swlist]->value == !(fake_fault->fault_type)) {
                    //Patterns found
                    
                    // fprintf(stdout, "T\'");
                    no_compression_patterns.clear();
                    switch (cktin[0]->value) {
                      case 0:
                      case D_bar:
                        no_compression_patterns.push_back("0");
                        // fprintf(stdout, "0");
                        break;
                      case 1:
                      case D:
                        no_compression_patterns.push_back("1");
                        // fprintf(stdout, "1");
                        break;
                      case U:
                        no_compression_patterns.push_back("0");
                        no_compression_patterns.push_back("1");
                        // fprintf(stdout, "x");
                        break;
                    }
                    
                    for (i = 1; i < cktin.size(); i++) {
                      switch (cktin[i]->value) {
                        case 0:
                        case D_bar:
                          // fprintf(stdout, "0");
                          for (auto& st : no_compression_patterns) {
                            st = st + "0";
                          }
                          break;
                        case 1:
                        case D:
                          // fprintf(stdout, "1");
                          for (auto& st : no_compression_patterns) {
                            st = st + "1";
                          }
                          break;
                        case U:
                          
                          if (no_compression_patterns.size()+total_no_compression_patterns.size() >= total_attempt_num) {
                            for (auto& st : no_compression_patterns) {
                              st = st + to_string(rand()&1);
                            }
                            break;
                          }
          
                          temp = no_compression_patterns;
                          for (auto& st : no_compression_patterns) {
                            st = st + "0";
                          }
                          for (auto st : temp) {
                            st = st + "1";
                            no_compression_patterns.push_back(st);
                          }
                          break;
                      }
                    }
                    // cout << " " << vec[0];
                    // fprintf(stdout, "'\n");
                    find_test = true; // if fault effect reaches PO, done. Fig 7.10
                    attempt_num++;
                    // no_compression_attempt_num += no_compression_patterns.size();
                    for (auto st : no_compression_patterns) {
                      if (vec[0] == 'x') {
                        st0 = st + " 0";
                        if (no_compression_patterns.size()+total_no_compression_patterns.size() >= total_attempt_num) {
                          st = st + " " + to_string(rand()&1);
                        }
                        else {
                          st = st + " 1";
                          total_no_compression_patterns.push_back(st0);
                        }
                      }
                      else {
                        st = st + " " + vec[0];
                      }
                      
                      total_no_compression_patterns.push_back(st);
                    }
                    decision_tree_for_pattern_1.clear();
                    for (i = cktin.size(); i < ncktwire; i++) {
                        sort_wlist[i]->value = U;
                    }
                    for (i = 0; i < cktin.size(); i++) {
                        sort_wlist[i]->value = ctoi(vec[i]); //change back to original first pattern
                        if (sort_wlist[i]->all_assigned) sort_wlist[i]->set_all_assigned();
                        else sort_wlist[i]->remove_all_assigned();
                    }
                    wpit = nullptr;
                    break;
                  }
                } // again
              } // while (one conditions)
            }
        }
        else {
            decision_tree_for_pattern_1.clear();
            for (i = cktin.size(); i < ncktwire; i++) {
                sort_wlist[i]->value = U;
            }
            for (i = 0; i < cktin.size(); i++) {
                sort_wlist[i]->value = ctoi(vec[i]); //change back to original first pattern
                if (sort_wlist[i]->all_assigned) sort_wlist[i]->set_all_assigned();
                else sort_wlist[i]->remove_all_assigned();
            }
            wpit = nullptr;
            
            for (wptr w: cktin) {
              w->set_changed();
            }
            sim();
        }
      }
      break;
    case CONFLICT:
      no_test = true; // cannot achieve initial objective, no test
      break;
    case FALSE:
      break;  //if no PI is reached, keep on backtracing. Fig 7.A
  }
  /* loop in Fig 7.ABC
   * quit the loop when either one of the three conditions is met:
   * 1. number of backtracks is equal to or larger than limit
   * 2. no_test
   * 3. already find a test pattern AND no_of_patterns meets required total_attempt_num */
  while ((no_of_backtracks < backtrack_limit) && !no_test &&
         !(find_test && (total_no_compression_patterns.size() >= total_attempt_num))) {
    /* check if test possible.   Fig. 7.1 */
    if (wpit = test_possible(fault)) {
      wpit->set_changed();
      /* insert a new PI into decision_tree */
      decision_tree.push_front(wpit);
    } else { // no test possible using this assignment, backtrack.
backtrack:
      if (nieh_speaks_details) cout << "V2 backtrack!\n";
      while (!decision_tree.empty() && (wpit == nullptr)) {
        /* if both 01 already tried, backtrack. Fig.7.7 */
        if (decision_tree.front()->is_all_assigned()) {
          decision_tree.front()->remove_all_assigned();  // clear the ALL_ASSIGNED flag
          decision_tree.front()->value = U; // do not assign 0 or 1
          decision_tree.front()->set_changed(); // this PI has been changed
          /* remove this PI in decision tree.  see dashed nodes in Fig 6 */
          decision_tree.pop_front();
        }
          /* else, flip last decision, flag ALL_ASSIGNED. Fig. 7.8 */
        else {
          decision_tree.front()->value = decision_tree.front()->value ^ 1; // flip last decision
          decision_tree.front()->set_changed(); // this PI has been changed
          decision_tree.front()->set_all_assigned();
          no_of_backtracks++;
          wpit = decision_tree.front();
        }
      } // while decision tree && ! wpit
      if (wpit == nullptr) no_test = true; //decision tree empty,  Fig 7.9
    } // no test possible

/* this again loop is to generate multiple patterns for a single fault 
 * this part is NOT in the original PODEM paper  */
    if (nieh_speaks_details) {
      cout << "V2: ";
      for (i = 0; i < cktin.size(); i++) {
          switch (cktin[i]->value) {
            case 0:
            case D_bar:
              fprintf(stdout, "0");
              break;
            case 1:
            case D:
              fprintf(stdout, "1");
              break;
            case U:
              fprintf(stdout, "x");
              break;
          }
      }
      cout << endl;
    }
    // again:
    if (wpit) {
      // cout << wpit->name << endl;
      sim();
      if (wfault = fault_evaluate(fault)) forward_imply(wfault);
      // for (i = 0; i < ncktwire; i++) {
      //   cout << sort_wlist[i]->name << " : " << sort_wlist[i]->value << endl;
      // }
      if (check_test()) {
        // if (total_attempt_num > 1) {
        //   display_io();
          
        // }
        // for (i=0; i<cktin.size();i++) {
        //         cout << i << " : " << cktin[i]->value << endl;
        //       }
        vec.clear();
        // decision_tree_for_pattern_1.clear();
        for (wptr w: cktin) {
            // cout << w->value << endl;
          vec.push_back(itoc(w->value));
          w->all_assigned = w->is_all_assigned();
          w->set_changed();
        //   cout << vec << endl;
        }
        for (i = cktin.size(); i < ncktwire; i++) {
          sort_wlist[i]->value = U;
        }
        for (i = 0; i < cktin.size()-1; i++) {
          sort_wlist[i]->value = ctoi(vec[i+1]); //LOS: find first pattern
        }
        sort_wlist[cktin.size()-1]->value = U;
        // cout << "Hi" << endl;
        // for (i=0; i<cktin.size();i++) {
        //     cout << i << " : " << cktin[i]->value << endl;
        // }
        // *fake_fault = *fault;
        // fake_fault->fault_type ^= 1;

        // cout << "name: " << sort_wlist[fake_fault->to_swlist]->name << endl;
        if (backward_imply(sort_wlist[fake_fault->to_swlist], !(fake_fault->fault_type)) != CONFLICT) {
            sim();  // Fig 7.3
            // cout << sort_wlist[fault->to_swlist]->value << " " << (fake_fault->fault_type) << endl;
            if (sort_wlist[fault->to_swlist]->value == (fake_fault->fault_type)) {
              // decision_tree_for_pattern_1.clear();
              for (i = cktin.size(); i < ncktwire; i++) {
                  sort_wlist[i]->value = U;
              }
              for (i = 0; i < cktin.size(); i++) {
                  sort_wlist[i]->value = ctoi(vec[i]); //change back to original first pattern
                  if (sort_wlist[i]->all_assigned) sort_wlist[i]->set_all_assigned();
                  else sort_wlist[i]->remove_all_assigned();
              }
              wpit = nullptr;
              for (wptr w: cktin) {
                w->set_changed();
              }
              sim();
              goto backtrack;
            }
            else if (sort_wlist[fault->to_swlist]->value == !(fake_fault->fault_type)) {
              //Patterns found
            //   for (i=0; i<cktin.size();i++) {
            //     cout << i << " : " << cktin[i]->value << endl;
            //   }
            //   cout << endl;
              
              // fprintf(stdout, "T\'");
              no_compression_patterns.clear();
              switch (cktin[0]->value) {
                case 0:
                case D_bar:
                  no_compression_patterns.push_back("0");
                  // fprintf(stdout, "0");
                  break;
                case 1:
                case D:
                  no_compression_patterns.push_back("1");
                  // fprintf(stdout, "1");
                  break;
                case U:
                  no_compression_patterns.push_back("0");
                  no_compression_patterns.push_back("1");
                  // fprintf(stdout, "x");
                  break;
              }
              
              for (i = 1; i < cktin.size(); i++) {
                switch (cktin[i]->value) {
                  case 0:
                  case D_bar:
                    // fprintf(stdout, "0");
                    for (auto& st : no_compression_patterns) {
                      st = st + "0";
                    }
                    break;
                  case 1:
                  case D:
                    // fprintf(stdout, "1");
                    for (auto& st : no_compression_patterns) {
                      st = st + "1";
                    }
                    break;
                  case U:
                    
                    if (no_compression_patterns.size()+total_no_compression_patterns.size() >= total_attempt_num) {
                      for (auto& st : no_compression_patterns) {
                        st = st + to_string(rand()&1);
                      }
                      break;
                    }
    
                    temp = no_compression_patterns;
                    for (auto& st : no_compression_patterns) {
                      st = st + "0";
                    }
                    for (auto st : temp) {
                      st = st + "1";
                      no_compression_patterns.push_back(st);
                    }
                    break;
                }
              }
              // cout << " " << vec[0];
              // fprintf(stdout, "'\n");
              find_test = true; // if fault effect reaches PO, done. Fig 7.10
              attempt_num++;
              // no_compression_attempt_num += no_compression_patterns.size();
              for (auto st : no_compression_patterns) {
                if (vec[0] == 'x') {
                  st0 = st + " 0";
                  if (no_compression_patterns.size()+total_no_compression_patterns.size() >= total_attempt_num) {
                    st = st + " " + to_string(rand()&1);
                  }
                  else {
                    st = st + " 1";
                    total_no_compression_patterns.push_back(st0);
                  }
                }
                else {
                  st = st + " " + vec[0];
                }
                
                total_no_compression_patterns.push_back(st);
              }
              decision_tree_for_pattern_1.clear();
              for (i = cktin.size(); i < ncktwire; i++) {
                sort_wlist[i]->value = U;
              }
              for (i = 0; i < cktin.size(); i++) {
                sort_wlist[i]->value = ctoi(vec[i]); //change back to original first pattern
                if (sort_wlist[i]->all_assigned) sort_wlist[i]->set_all_assigned();
                else sort_wlist[i]->remove_all_assigned();
              }
              wpit = nullptr;
              
              for (wptr w: cktin) {
                w->set_changed();
              }
              sim();
              if (attempt_num != total_attempt_num) goto backtrack;
            }
            else {
              no_of_backtracks_1 = 0;
              while ((no_of_backtracks_1 < backtrack_limit)) {
                /* check if test possible.   Fig. 7.1 */
                if (wpit = test_possible(fake_fault)) {
                  wpit->remove_all_assigned();
                  wpit->set_changed();
                /* insert a new PI into decision_tree */
                  decision_tree_for_pattern_1.push_front(wpit);
                  // cout << wpit->wlist_index << endl;
                } else { // no test possible using this assignment, backtrack.
                  if (nieh_speaks_details) cout << "V1 backtrack!\n";
                  while (!decision_tree_for_pattern_1.empty() && (wpit == nullptr)) {
                    /* if both 01 already tried, backtrack. Fig.7.7 */
                    if (decision_tree_for_pattern_1.front()->is_all_assigned()) {
                      decision_tree_for_pattern_1.front()->remove_all_assigned();  // clear the ALL_ASSIGNED flag
                      decision_tree_for_pattern_1.front()->value = U; // do not assign 0 or 1
                      decision_tree_for_pattern_1.front()->set_changed(); // this PI has been changed
                    /* remove this PI in decision tree.  see dashed nodes in Fig 6 */
                      decision_tree_for_pattern_1.pop_front();
                    }
                    /* else, flip last decision, flag ALL_ASSIGNED. Fig. 7.8 */
                    else {
                      decision_tree_for_pattern_1.front()->value = decision_tree_for_pattern_1.front()->value ^ 1; // flip last decision
                      decision_tree_for_pattern_1.front()->set_changed(); // this PI has been changed
                      decision_tree_for_pattern_1.front()->set_all_assigned();
                      no_of_backtracks_1++;
                      wpit = decision_tree_for_pattern_1.front();
                    }
                  } // while decision tree && ! wpit
                  
                } // no test possible

            /* this again loop is to generate multiple patterns for a single fault 
            * this part is NOT in the original PODEM paper  */


                // again:
                if (wpit == nullptr) {
                  decision_tree_for_pattern_1.clear();
                  for (i = cktin.size(); i < ncktwire; i++) {
                      sort_wlist[i]->value = U;
                  }
                  for (i = 0; i < cktin.size(); i++) {
                      sort_wlist[i]->value = ctoi(vec[i]); //change back to original first pattern
                      if (sort_wlist[i]->all_assigned) sort_wlist[i]->set_all_assigned();
                      else sort_wlist[i]->remove_all_assigned();
                  }
                  wpit = nullptr;
                  
                  for (wptr w: cktin) {
                    w->set_changed();
                  }
                  sim();
                  goto backtrack;
                } //try other pattern 2
                else { //if (wpit)
if (nieh_speaks_details) {
  cout << "V1: ";
  for (int h = 0; h < cktin.size(); h++) {
      switch (cktin[h]->value) {
        case 0:
        case D_bar:
          fprintf(stdout, "0");
          break;
        case 1:
        case D:
          fprintf(stdout, "1");
          break;
        case U:
          fprintf(stdout, "x");
          break;
      }
  }
  fprintf(stdout, "\n");
}
                // cout << wpit->name << endl;
                  sim();
                  if (sort_wlist[fault->to_swlist]->value == !(fake_fault->fault_type)) {
                    if (nieh_speaks_details) cout << "Patterns found\n";
                    
                    // fprintf(stdout, "T\'");
                    no_compression_patterns.clear();
                    switch (cktin[0]->value) {
                      case 0:
                      case D_bar:
                        no_compression_patterns.push_back("0");
                        // fprintf(stdout, "0");
                        break;
                      case 1:
                      case D:
                        no_compression_patterns.push_back("1");
                        // fprintf(stdout, "1");
                        break;
                      case U:
                        no_compression_patterns.push_back("0");
                        no_compression_patterns.push_back("1");
                        // fprintf(stdout, "x");
                        break;
                    }
                    
                    for (i = 1; i < cktin.size(); i++) {
                      switch (cktin[i]->value) {
                        case 0:
                        case D_bar:
                          // fprintf(stdout, "0");
                          for (auto& st : no_compression_patterns) {
                            st = st + "0";
                          }
                          break;
                        case 1:
                        case D:
                          // fprintf(stdout, "1");
                          for (auto& st : no_compression_patterns) {
                            st = st + "1";
                          }
                          break;
                        case U:
                          
                          if (no_compression_patterns.size()+total_no_compression_patterns.size() >= total_attempt_num) {
                            for (auto& st : no_compression_patterns) {
                              st = st + to_string(rand()&1);
                            }
                            break;
                          }
          
                          temp = no_compression_patterns;
                          for (auto& st : no_compression_patterns) {
                            st = st + "0";
                          }
                          for (auto st : temp) {
                            st = st + "1";
                            no_compression_patterns.push_back(st);
                          }
                          break;
                      }
                    }
                    // cout << " " << vec[0];
                    // fprintf(stdout, "'\n");
                    find_test = true; // if fault effect reaches PO, done. Fig 7.10
                    attempt_num++;
                    // no_compression_attempt_num += no_compression_patterns.size();
                    for (auto st : no_compression_patterns) {
                      if (vec[0] == 'x') {
                        st0 = st + " 0";
                        if (no_compression_patterns.size()+total_no_compression_patterns.size() >= total_attempt_num) {
                          st = st + " " + to_string(rand()&1);
                        }
                        else {
                          st = st + " 1";
                          total_no_compression_patterns.push_back(st0);
                        }
                      }
                      else {
                        st = st + " " + vec[0];
                      }
                      
                      total_no_compression_patterns.push_back(st);
                    }
                    decision_tree_for_pattern_1.clear();
                    for (i = cktin.size(); i < ncktwire; i++) {
                        sort_wlist[i]->value = U;
                    }
                    for (i = 0; i < cktin.size(); i++) {
                        sort_wlist[i]->value = ctoi(vec[i]); //change back to original first pattern
                        if (sort_wlist[i]->all_assigned) sort_wlist[i]->set_all_assigned();
                        else sort_wlist[i]->remove_all_assigned();
                    }
                    wpit = nullptr;
                    // cout << "GO " << attempt_num << " " << total_attempt_num << endl;
                    
                    for (wptr w: cktin) {
                      w->set_changed();
                    }
                    sim();
                    if (attempt_num != total_attempt_num) goto backtrack;
                    else break;
                    // cout << "Invisible" << endl;
                  }
                } // again
              } // while (one conditions)
            }
        }
        else {
            decision_tree_for_pattern_1.clear();
            for (i = cktin.size(); i < ncktwire; i++) {
                sort_wlist[i]->value = U;
            }
            for (i = 0; i < cktin.size(); i++) {
                sort_wlist[i]->value = ctoi(vec[i]); //change back to original first pattern
                if (sort_wlist[i]->all_assigned) sort_wlist[i]->set_all_assigned();
                else sort_wlist[i]->remove_all_assigned();
            }
            wpit = nullptr;
            
            for (wptr w: cktin) {
              w->set_changed();
            }
            sim();
            goto backtrack;
        }
      }  // if check_test()
    } // again
    // cout << "attempt: " << total_no_compression_patterns.size() << " " << total_attempt_num << endl;
    // cout << "backtrack: " << no_of_backtracks << " " << backtrack_limit << endl;
    
  } // while (three conditions)
  /* clear everything */
  for (wptr wptr_ele: decision_tree) {
    wptr_ele->remove_all_assigned();
  }
  decision_tree.clear();

  current_backtracks = no_of_backtracks;
  unmark_propagate_tree(fault->node);
  
  random_index.clear();
  if (no_of_backtracks >= backtrack_limit) {
    if (nieh_speaks) cout << "Backtrack limit is hit." << endl;
  }
  if (total_no_compression_patterns.size() < total_attempt_num) {
    if (nieh_speaks) {
      
      fprintf(stdout,"redundant fault...\n\n");
    }
    //print anyway
    in_vector_no += total_no_compression_patterns.size();
    for (auto st : total_no_compression_patterns) {
      cout << "T\'" << st << "\'\n";
    }
  }
  else {
    for (y = 0; y < total_attempt_num; y++) {
      r = rand()%total_no_compression_patterns.size();
      while (std::find(random_index.begin(), random_index.end(), r) != random_index.end()) { //index repeat
      // cout << random_index.size() << endl;
        r = rand()%total_no_compression_patterns.size();
      }
      random_index.push_back(r);
    }
    in_vector_no += random_index.size();
    for (auto g : random_index) {
      // cout << g << endl;
      vectors.push_back(total_no_compression_patterns[g]);
      cout << "T\'" << total_no_compression_patterns[g] << "\'\n";
    }
    if (nieh_speaks) cout << "\n";
    return (TRUE);
  }
  return (FALSE);
}/* end of tdfpodem */

int ATPG::tdf2xpodem(const fptr fault, int &current_backtracks) {
  bool nieh_speaks = false;
  bool nieh_speaks_details = false;
  string vec;
  int i, ncktwire, ncktin, r, y;
  forward_list<wptr> decision_tree; // design_tree (a LIFO stack)
  wptr wfault;
  int attempt_num = 0;  // counts the number of pattern generated so far for the given fault
  if (nieh_speaks) display_fault(fault);
  forward_list<wptr> decision_tree_for_pattern_1; // design_tree (a LIFO stack)
  int no_of_backtracks_1;
  *fake_fault = *fault;
  fake_fault->fault_type ^= 1;
  vector<string> total_no_compression_patterns;
  vector<string> no_compression_patterns;
  vector<string> temp;
  string st0;
  // int no_compression_attempt_num = 0;  // counts the number of pattern generated so far for the given fault
  vector<int> random_index;

  /* initialize all circuit wires to unknown */
  ncktwire = sort_wlist.size();
  ncktin = cktin.size();
  for (i = 0; i < ncktwire; i++) {
    sort_wlist[i]->value = U;
  }
  no_of_backtracks = 0;
  find_test = false;
  no_test = false;

  mark_propagate_tree(fault->node);

  /* Fig 7 starts here */
  /* set the initial objective, assign the first PI.  Fig 7.P1 */
  switch (set_uniquely_implied_value(fault)) {
    case TRUE: // if a  PI is assigned
      sim();  // Fig 7.3
      wfault = fault_evaluate(fault);
      if (wfault != nullptr) forward_imply(wfault);// propagate fault effect
      if (check_test()) {
        // if (total_attempt_num > 1) {
        //   display_io();
          
        // }
        vec.clear();
        for (wptr w: cktin) {
          vec.push_back(itoc(w->value));
          w->all_assigned = w->is_all_assigned();
        }
        for (i = cktin.size(); i < ncktwire; i++) {
          sort_wlist[i]->value = U;
        }
        for (i = 0; i < cktin.size()-1; i++) {
          sort_wlist[i]->value = ctoi(vec[i+1]); //LOS: find first pattern
        }
        sort_wlist[cktin.size()-1]->value = U;
        // *fake_fault = *fault;
        // fake_fault->fault_type ^= 1;
        if (backward_imply(sort_wlist[fake_fault->to_swlist], !(fake_fault->fault_type)) != CONFLICT) {
            sim();  // Fig 7.3
            if (sort_wlist[fault->to_swlist]->value == (fake_fault->fault_type)) {
              // decision_tree_for_pattern_1.clear();
              for (i = cktin.size(); i < ncktwire; i++) {
                  sort_wlist[i]->value = U;
              }
              for (i = 0; i < cktin.size(); i++) {
                  sort_wlist[i]->value = ctoi(vec[i]); //change back to original first pattern
                  if (sort_wlist[i]->all_assigned) sort_wlist[i]->set_all_assigned();
                  else sort_wlist[i]->remove_all_assigned();
              }
              wpit = nullptr;
              for (wptr w: cktin) {
                w->set_changed();
              }
              sim();
              goto backtrack;
            }
            else if (sort_wlist[fault->to_swlist]->value == !(fake_fault->fault_type)) {
              //Patterns found
              // fprintf(stdout, "T\'");
              no_compression_patterns.clear();
              switch (cktin[0]->value) {
                case 0:
                case D_bar:
                  no_compression_patterns.push_back("0");
                  // fprintf(stdout, "0");
                  break;
                case 1:
                case D:
                  no_compression_patterns.push_back("1");
                  // fprintf(stdout, "1");
                  break;
                case U:
                  no_compression_patterns.push_back("0");
                  no_compression_patterns.push_back("1");
                  // fprintf(stdout, "x");
                  break;
              }
              
              for (i = 1; i < cktin.size(); i++) {
                switch (cktin[i]->value) {
                  case 0:
                  case D_bar:
                    // fprintf(stdout, "0");
                    for (auto& st : no_compression_patterns) {
                      st = st + "0";
                    }
                    break;
                  case 1:
                  case D:
                    // fprintf(stdout, "1");
                    for (auto& st : no_compression_patterns) {
                      st = st + "1";
                    }
                    break;
                  case U:
                    // fprintf(stdout, "x");
                    
                    if (no_compression_patterns.size()+total_no_compression_patterns.size() >= total_attempt_num) {
                      for (auto& st : no_compression_patterns) {
                        st = st + to_string(rand()&1);
                      }
                      break;
                    }
    
                    temp = no_compression_patterns;
                    for (auto& st : no_compression_patterns) {
                      st = st + "0";
                    }
                    for (auto st : temp) {
                      st = st + "1";
                      no_compression_patterns.push_back(st);
                    }
                    break;
                }
              }
              // cout << " " << vec[0];
              // fprintf(stdout, "'\n");
              find_test = true; // if fault effect reaches PO, done. Fig 7.10
              attempt_num++;
              // no_compression_attempt_num += no_compression_patterns.size();
              for (auto st : no_compression_patterns) {
                if (vec[0] == 'x') {
                  st0 = st + " 0";
                  if (no_compression_patterns.size()+total_no_compression_patterns.size() >= total_attempt_num) {
                    st = st + " " + to_string(rand()&1);
                  }
                  else {
                    st = st + " 1";
                    total_no_compression_patterns.push_back(st0);
                  }
                }
                else {
                  st = st + " " + vec[0];
                }
                
                total_no_compression_patterns.push_back(st);
              }
              decision_tree_for_pattern_1.clear();
              for (i = cktin.size(); i < ncktwire; i++) {
                sort_wlist[i]->value = U;
              }
              for (i = 0; i < cktin.size(); i++) {
                sort_wlist[i]->value = ctoi(vec[i]); //change back to original first pattern
                if (sort_wlist[i]->all_assigned) sort_wlist[i]->set_all_assigned();
                else sort_wlist[i]->remove_all_assigned();
              }
              wpit = nullptr;
              
              // goto backtrack;
            }
            else {
              no_of_backtracks_1 = 0;
              while ((no_of_backtracks_1 < backtrack_limit)) {
                /* check if test possible.   Fig. 7.1 */
                if (wpit = test_possible(fake_fault)) {
                  wpit->remove_all_assigned();
                  wpit->set_changed();
                /* insert a new PI into decision_tree */
                  decision_tree_for_pattern_1.push_front(wpit);
                } else { // no test possible using this assignment, backtrack.
                  while (!decision_tree_for_pattern_1.empty() && (wpit == nullptr)) {
                    /* if both 01 already tried, backtrack. Fig.7.7 */
                    if (decision_tree_for_pattern_1.front()->is_all_assigned()) {
                      decision_tree_for_pattern_1.front()->remove_all_assigned();  // clear the ALL_ASSIGNED flag
                      decision_tree_for_pattern_1.front()->value = U; // do not assign 0 or 1
                      decision_tree_for_pattern_1.front()->set_changed(); // this PI has been changed
                    /* remove this PI in decision tree.  see dashed nodes in Fig 6 */
                      decision_tree_for_pattern_1.pop_front();
                    }
                    /* else, flip last decision, flag ALL_ASSIGNED. Fig. 7.8 */
                    else {
                      decision_tree_for_pattern_1.front()->value = decision_tree_for_pattern_1.front()->value ^ 1; // flip last decision
                      decision_tree_for_pattern_1.front()->set_changed(); // this PI has been changed
                      decision_tree_for_pattern_1.front()->set_all_assigned();
                      no_of_backtracks_1++;
                      wpit = decision_tree_for_pattern_1.front();
                    }
                  } // while decision tree && ! wpit
                  
                } // no test possible

            /* this again loop is to generate multiple patterns for a single fault 
            * this part is NOT in the original PODEM paper  */

                if (wpit == nullptr) {
                  decision_tree_for_pattern_1.clear();
                  for (i = cktin.size(); i < ncktwire; i++) {
                      sort_wlist[i]->value = U;
                  }
                  for (i = 0; i < cktin.size(); i++) {
                      sort_wlist[i]->value = ctoi(vec[i]); //change back to original first pattern
                      if (sort_wlist[i]->all_assigned) sort_wlist[i]->set_all_assigned();
                      else sort_wlist[i]->remove_all_assigned();
                  }
                  break;
                  // goto backtrack;
                } //try other pattern 2
                // again:
                else { //if (wpit)
                // cout << wpit->name << endl;
                  sim();
                  if (sort_wlist[fault->to_swlist]->value == !(fake_fault->fault_type)) {
                    //Patterns found
                    
                    // fprintf(stdout, "T\'");
                    no_compression_patterns.clear();
                    switch (cktin[0]->value) {
                      case 0:
                      case D_bar:
                        no_compression_patterns.push_back("0");
                        // fprintf(stdout, "0");
                        break;
                      case 1:
                      case D:
                        no_compression_patterns.push_back("1");
                        // fprintf(stdout, "1");
                        break;
                      case U:
                        no_compression_patterns.push_back("0");
                        no_compression_patterns.push_back("1");
                        // fprintf(stdout, "x");
                        break;
                    }
                    
                    for (i = 1; i < cktin.size(); i++) {
                      switch (cktin[i]->value) {
                        case 0:
                        case D_bar:
                          // fprintf(stdout, "0");
                          for (auto& st : no_compression_patterns) {
                            st = st + "0";
                          }
                          break;
                        case 1:
                        case D:
                          // fprintf(stdout, "1");
                          for (auto& st : no_compression_patterns) {
                            st = st + "1";
                          }
                          break;
                        case U:
                          
                          if (no_compression_patterns.size()+total_no_compression_patterns.size() >= total_attempt_num) {
                            for (auto& st : no_compression_patterns) {
                              st = st + to_string(rand()&1);
                            }
                            break;
                          }
          
                          temp = no_compression_patterns;
                          for (auto& st : no_compression_patterns) {
                            st = st + "0";
                          }
                          for (auto st : temp) {
                            st = st + "1";
                            no_compression_patterns.push_back(st);
                          }
                          break;
                      }
                    }
                    // cout << " " << vec[0];
                    // fprintf(stdout, "'\n");
                    find_test = true; // if fault effect reaches PO, done. Fig 7.10
                    attempt_num++;
                    // no_compression_attempt_num += no_compression_patterns.size();
                    for (auto st : no_compression_patterns) {
                      if (vec[0] == 'x') {
                        st0 = st + " 0";
                        if (no_compression_patterns.size()+total_no_compression_patterns.size() >= total_attempt_num) {
                          st = st + " " + to_string(rand()&1);
                        }
                        else {
                          st = st + " 1";
                          total_no_compression_patterns.push_back(st0);
                        }
                      }
                      else {
                        st = st + " " + vec[0];
                      }
                      
                      total_no_compression_patterns.push_back(st);
                    }
                    decision_tree_for_pattern_1.clear();
                    for (i = cktin.size(); i < ncktwire; i++) {
                        sort_wlist[i]->value = U;
                    }
                    for (i = 0; i < cktin.size(); i++) {
                        sort_wlist[i]->value = ctoi(vec[i]); //change back to original first pattern
                        if (sort_wlist[i]->all_assigned) sort_wlist[i]->set_all_assigned();
                        else sort_wlist[i]->remove_all_assigned();
                    }
                    wpit = nullptr;
                    break;
                  }
                } // again
              } // while (one conditions)
            }
        }
        else {
            decision_tree_for_pattern_1.clear();
            for (i = cktin.size(); i < ncktwire; i++) {
                sort_wlist[i]->value = U;
            }
            for (i = 0; i < cktin.size(); i++) {
                sort_wlist[i]->value = ctoi(vec[i]); //change back to original first pattern
                if (sort_wlist[i]->all_assigned) sort_wlist[i]->set_all_assigned();
                else sort_wlist[i]->remove_all_assigned();
            }
            wpit = nullptr;
            
            for (wptr w: cktin) {
              w->set_changed();
            }
            sim();
        }
      }
      break;
    case CONFLICT:
      no_test = true; // cannot achieve initial objective, no test
      break;
    case FALSE:
      break;  //if no PI is reached, keep on backtracing. Fig 7.A
  }
  /* loop in Fig 7.ABC
   * quit the loop when either one of the three conditions is met:
   * 1. number of backtracks is equal to or larger than limit
   * 2. no_test
   * 3. already find a test pattern AND no_of_patterns meets required total_attempt_num */
  while ((no_of_backtracks < backtrack_limit) && !no_test &&
         !(find_test && (total_no_compression_patterns.size() >= total_attempt_num))) {
    /* check if test possible.   Fig. 7.1 */
    if (wpit = test_possible(fault)) {
      wpit->set_changed();
      /* insert a new PI into decision_tree */
      decision_tree.push_front(wpit);
    } else { // no test possible using this assignment, backtrack.
backtrack:
      if (nieh_speaks_details) cout << "V2 backtrack!\n";
      while (!decision_tree.empty() && (wpit == nullptr)) {
        /* if both 01 already tried, backtrack. Fig.7.7 */
        if (decision_tree.front()->is_all_assigned()) {
          decision_tree.front()->remove_all_assigned();  // clear the ALL_ASSIGNED flag
          decision_tree.front()->value = U; // do not assign 0 or 1
          decision_tree.front()->set_changed(); // this PI has been changed
          /* remove this PI in decision tree.  see dashed nodes in Fig 6 */
          decision_tree.pop_front();
        }
          /* else, flip last decision, flag ALL_ASSIGNED. Fig. 7.8 */
        else {
          decision_tree.front()->value = decision_tree.front()->value ^ 1; // flip last decision
          decision_tree.front()->set_changed(); // this PI has been changed
          decision_tree.front()->set_all_assigned();
          no_of_backtracks++;
          wpit = decision_tree.front();
        }
      } // while decision tree && ! wpit
      if (wpit == nullptr) no_test = true; //decision tree empty,  Fig 7.9
    } // no test possible

/* this again loop is to generate multiple patterns for a single fault 
 * this part is NOT in the original PODEM paper  */
    if (nieh_speaks_details) {
      cout << "V2: ";
      for (i = 0; i < cktin.size(); i++) {
          switch (cktin[i]->value) {
            case 0:
            case D_bar:
              fprintf(stdout, "0");
              break;
            case 1:
            case D:
              fprintf(stdout, "1");
              break;
            case U:
              fprintf(stdout, "x");
              break;
          }
      }
      cout << endl;
    }
    // again:
    if (wpit) {
      // cout << wpit->name << endl;
      sim();
      if (wfault = fault_evaluate(fault)) forward_imply(wfault);
      // for (i = 0; i < ncktwire; i++) {
      //   cout << sort_wlist[i]->name << " : " << sort_wlist[i]->value << endl;
      // }
      if (check_test()) {
        // if (total_attempt_num > 1) {
        //   display_io();
          
        // }
        // for (i=0; i<cktin.size();i++) {
        //         cout << i << " : " << cktin[i]->value << endl;
        //       }
        vec.clear();
        // decision_tree_for_pattern_1.clear();
        for (wptr w: cktin) {
            // cout << w->value << endl;
          vec.push_back(itoc(w->value));
          w->all_assigned = w->is_all_assigned();
          w->set_changed();
        //   cout << vec << endl;
        }
        for (i = cktin.size(); i < ncktwire; i++) {
          sort_wlist[i]->value = U;
        }
        for (i = 0; i < cktin.size()-1; i++) {
          sort_wlist[i]->value = ctoi(vec[i+1]); //LOS: find first pattern
        }
        sort_wlist[cktin.size()-1]->value = U;
        // cout << "Hi" << endl;
        // for (i=0; i<cktin.size();i++) {
        //     cout << i << " : " << cktin[i]->value << endl;
        // }
        // *fake_fault = *fault;
        // fake_fault->fault_type ^= 1;

        // cout << "name: " << sort_wlist[fake_fault->to_swlist]->name << endl;
        if (backward_imply(sort_wlist[fake_fault->to_swlist], !(fake_fault->fault_type)) != CONFLICT) {
            sim();  // Fig 7.3
            // cout << sort_wlist[fault->to_swlist]->value << " " << (fake_fault->fault_type) << endl;
            if (sort_wlist[fault->to_swlist]->value == (fake_fault->fault_type)) {
              // decision_tree_for_pattern_1.clear();
              for (i = cktin.size(); i < ncktwire; i++) {
                  sort_wlist[i]->value = U;
              }
              for (i = 0; i < cktin.size(); i++) {
                  sort_wlist[i]->value = ctoi(vec[i]); //change back to original first pattern
                  if (sort_wlist[i]->all_assigned) sort_wlist[i]->set_all_assigned();
                  else sort_wlist[i]->remove_all_assigned();
              }
              wpit = nullptr;
              for (wptr w: cktin) {
                w->set_changed();
              }
              sim();
              goto backtrack;
            }
            else if (sort_wlist[fault->to_swlist]->value == !(fake_fault->fault_type)) {
              //Patterns found
            //   for (i=0; i<cktin.size();i++) {
            //     cout << i << " : " << cktin[i]->value << endl;
            //   }
            //   cout << endl;
              
              // fprintf(stdout, "T\'");
              no_compression_patterns.clear();
              switch (cktin[0]->value) {
                case 0:
                case D_bar:
                  no_compression_patterns.push_back("0");
                  // fprintf(stdout, "0");
                  break;
                case 1:
                case D:
                  no_compression_patterns.push_back("1");
                  // fprintf(stdout, "1");
                  break;
                case U:
                  no_compression_patterns.push_back("0");
                  no_compression_patterns.push_back("1");
                  // fprintf(stdout, "x");
                  break;
              }
              
              for (i = 1; i < cktin.size(); i++) {
                switch (cktin[i]->value) {
                  case 0:
                  case D_bar:
                    // fprintf(stdout, "0");
                    for (auto& st : no_compression_patterns) {
                      st = st + "0";
                    }
                    break;
                  case 1:
                  case D:
                    // fprintf(stdout, "1");
                    for (auto& st : no_compression_patterns) {
                      st = st + "1";
                    }
                    break;
                  case U:
                    
                    if (no_compression_patterns.size()+total_no_compression_patterns.size() >= total_attempt_num) {
                      for (auto& st : no_compression_patterns) {
                        st = st + to_string(rand()&1);
                      }
                      break;
                    }
    
                    temp = no_compression_patterns;
                    for (auto& st : no_compression_patterns) {
                      st = st + "0";
                    }
                    for (auto st : temp) {
                      st = st + "1";
                      no_compression_patterns.push_back(st);
                    }
                    break;
                }
              }
              // cout << " " << vec[0];
              // fprintf(stdout, "'\n");
              find_test = true; // if fault effect reaches PO, done. Fig 7.10
              attempt_num++;
              // no_compression_attempt_num += no_compression_patterns.size();
              for (auto st : no_compression_patterns) {
                if (vec[0] == 'x') {
                  st0 = st + " 0";
                  if (no_compression_patterns.size()+total_no_compression_patterns.size() >= total_attempt_num) {
                    st = st + " " + to_string(rand()&1);
                  }
                  else {
                    st = st + " 1";
                    total_no_compression_patterns.push_back(st0);
                  }
                }
                else {
                  st = st + " " + vec[0];
                }
                
                total_no_compression_patterns.push_back(st);
              }
              decision_tree_for_pattern_1.clear();
              for (i = cktin.size(); i < ncktwire; i++) {
                sort_wlist[i]->value = U;
              }
              for (i = 0; i < cktin.size(); i++) {
                sort_wlist[i]->value = ctoi(vec[i]); //change back to original first pattern
                if (sort_wlist[i]->all_assigned) sort_wlist[i]->set_all_assigned();
                else sort_wlist[i]->remove_all_assigned();
              }
              wpit = nullptr;
              
              for (wptr w: cktin) {
                w->set_changed();
              }
              sim();
              if (attempt_num != total_attempt_num) goto backtrack;
            }
            else {
              no_of_backtracks_1 = 0;
              while ((no_of_backtracks_1 < backtrack_limit)) {
                /* check if test possible.   Fig. 7.1 */
                if (wpit = test_possible(fake_fault)) {
                  wpit->remove_all_assigned();
                  wpit->set_changed();
                /* insert a new PI into decision_tree */
                  decision_tree_for_pattern_1.push_front(wpit);
                  // cout << wpit->wlist_index << endl;
                } else { // no test possible using this assignment, backtrack.
                  if (nieh_speaks_details) cout << "V1 backtrack!\n";
                  while (!decision_tree_for_pattern_1.empty() && (wpit == nullptr)) {
                    /* if both 01 already tried, backtrack. Fig.7.7 */
                    if (decision_tree_for_pattern_1.front()->is_all_assigned()) {
                      decision_tree_for_pattern_1.front()->remove_all_assigned();  // clear the ALL_ASSIGNED flag
                      decision_tree_for_pattern_1.front()->value = U; // do not assign 0 or 1
                      decision_tree_for_pattern_1.front()->set_changed(); // this PI has been changed
                    /* remove this PI in decision tree.  see dashed nodes in Fig 6 */
                      decision_tree_for_pattern_1.pop_front();
                    }
                    /* else, flip last decision, flag ALL_ASSIGNED. Fig. 7.8 */
                    else {
                      decision_tree_for_pattern_1.front()->value = decision_tree_for_pattern_1.front()->value ^ 1; // flip last decision
                      decision_tree_for_pattern_1.front()->set_changed(); // this PI has been changed
                      decision_tree_for_pattern_1.front()->set_all_assigned();
                      no_of_backtracks_1++;
                      wpit = decision_tree_for_pattern_1.front();
                    }
                  } // while decision tree && ! wpit
                  
                } // no test possible

            /* this again loop is to generate multiple patterns for a single fault 
            * this part is NOT in the original PODEM paper  */


                // again:
                if (wpit == nullptr) {
                  decision_tree_for_pattern_1.clear();
                  for (i = cktin.size(); i < ncktwire; i++) {
                      sort_wlist[i]->value = U;
                  }
                  for (i = 0; i < cktin.size(); i++) {
                      sort_wlist[i]->value = ctoi(vec[i]); //change back to original first pattern
                      if (sort_wlist[i]->all_assigned) sort_wlist[i]->set_all_assigned();
                      else sort_wlist[i]->remove_all_assigned();
                  }
                  wpit = nullptr;
                  
                  for (wptr w: cktin) {
                    w->set_changed();
                  }
                  sim();
                  goto backtrack;
                } //try other pattern 2
                else { //if (wpit)
if (nieh_speaks_details) {
  cout << "V1: ";
  for (int h = 0; h < cktin.size(); h++) {
      switch (cktin[h]->value) {
        case 0:
        case D_bar:
          fprintf(stdout, "0");
          break;
        case 1:
        case D:
          fprintf(stdout, "1");
          break;
        case U:
          fprintf(stdout, "x");
          break;
      }
  }
  fprintf(stdout, "\n");
}
                // cout << wpit->name << endl;
                  sim();
                  if (sort_wlist[fault->to_swlist]->value == !(fake_fault->fault_type)) {
                    if (nieh_speaks_details) cout << "Patterns found\n";
                    
                    // fprintf(stdout, "T\'");
                    no_compression_patterns.clear();
                    switch (cktin[0]->value) {
                      case 0:
                      case D_bar:
                        no_compression_patterns.push_back("0");
                        // fprintf(stdout, "0");
                        break;
                      case 1:
                      case D:
                        no_compression_patterns.push_back("1");
                        // fprintf(stdout, "1");
                        break;
                      case U:
                        no_compression_patterns.push_back("0");
                        no_compression_patterns.push_back("1");
                        // fprintf(stdout, "x");
                        break;
                    }
                    
                    for (i = 1; i < cktin.size(); i++) {
                      switch (cktin[i]->value) {
                        case 0:
                        case D_bar:
                          // fprintf(stdout, "0");
                          for (auto& st : no_compression_patterns) {
                            st = st + "0";
                          }
                          break;
                        case 1:
                        case D:
                          // fprintf(stdout, "1");
                          for (auto& st : no_compression_patterns) {
                            st = st + "1";
                          }
                          break;
                        case U:
                          
                          if (no_compression_patterns.size()+total_no_compression_patterns.size() >= total_attempt_num) {
                            for (auto& st : no_compression_patterns) {
                              st = st + to_string(rand()&1);
                            }
                            break;
                          }
          
                          temp = no_compression_patterns;
                          for (auto& st : no_compression_patterns) {
                            st = st + "0";
                          }
                          for (auto st : temp) {
                            st = st + "1";
                            no_compression_patterns.push_back(st);
                          }
                          break;
                      }
                    }
                    // cout << " " << vec[0];
                    // fprintf(stdout, "'\n");
                    find_test = true; // if fault effect reaches PO, done. Fig 7.10
                    attempt_num++;
                    // no_compression_attempt_num += no_compression_patterns.size();
                    for (auto st : no_compression_patterns) {
                      if (vec[0] == 'x') {
                        st0 = st + " 0";
                        if (no_compression_patterns.size()+total_no_compression_patterns.size() >= total_attempt_num) {
                          st = st + " " + to_string(rand()&1);
                        }
                        else {
                          st = st + " 1";
                          total_no_compression_patterns.push_back(st0);
                        }
                      }
                      else {
                        st = st + " " + vec[0];
                      }
                      
                      total_no_compression_patterns.push_back(st);
                    }
                    decision_tree_for_pattern_1.clear();
                    for (i = cktin.size(); i < ncktwire; i++) {
                        sort_wlist[i]->value = U;
                    }
                    for (i = 0; i < cktin.size(); i++) {
                        sort_wlist[i]->value = ctoi(vec[i]); //change back to original first pattern
                        if (sort_wlist[i]->all_assigned) sort_wlist[i]->set_all_assigned();
                        else sort_wlist[i]->remove_all_assigned();
                    }
                    wpit = nullptr;
                    // cout << "GO " << attempt_num << " " << total_attempt_num << endl;
                    
                    for (wptr w: cktin) {
                      w->set_changed();
                    }
                    sim();
                    if (attempt_num != total_attempt_num) goto backtrack;
                    else break;
                    // cout << "Invisible" << endl;
                  }
                } // again
              } // while (one conditions)
            }
        }
        else {
            decision_tree_for_pattern_1.clear();
            for (i = cktin.size(); i < ncktwire; i++) {
                sort_wlist[i]->value = U;
            }
            for (i = 0; i < cktin.size(); i++) {
                sort_wlist[i]->value = ctoi(vec[i]); //change back to original first pattern
                if (sort_wlist[i]->all_assigned) sort_wlist[i]->set_all_assigned();
                else sort_wlist[i]->remove_all_assigned();
            }
            wpit = nullptr;
            
            for (wptr w: cktin) {
              w->set_changed();
            }
            sim();
            goto backtrack;
        }
      }  // if check_test()
    } // again
    // cout << "attempt: " << total_no_compression_patterns.size() << " " << total_attempt_num << endl;
    // cout << "backtrack: " << no_of_backtracks << " " << backtrack_limit << endl;
    
  } // while (three conditions)
  /* clear everything */
  for (wptr wptr_ele: decision_tree) {
    wptr_ele->remove_all_assigned();
  }
  decision_tree.clear();

  current_backtracks = no_of_backtracks;
  unmark_propagate_tree(fault->node);
  
  random_index.clear();
  if (no_of_backtracks >= backtrack_limit) {
    if (nieh_speaks) cout << "Backtrack limit is hit." << endl;
  }
  if (total_no_compression_patterns.size() < total_attempt_num) {
    if (nieh_speaks) {
      
      fprintf(stdout,"redundant fault...\n\n");
    }
    //print anyway
    in_vector_no += total_no_compression_patterns.size();
    for (auto st : total_no_compression_patterns) {
      cout << "T\'" << st << "\'\n";
    }
  }
  else {
    tdf2xpodem_dtc(current_backtracks);
    for (y = 0; y < total_attempt_num; y++) {
      r = rand()%total_no_compression_patterns.size();
      while (std::find(random_index.begin(), random_index.end(), r) != random_index.end()) { //index repeat
      // cout << random_index.size() << endl;
        r = rand()%total_no_compression_patterns.size();
      }
      random_index.push_back(r);
    }
    in_vector_no += random_index.size();
    for (auto g : random_index) {
      // cout << g << endl;
      vectors.push_back(total_no_compression_patterns[g]);
      cout << "T\'" << total_no_compression_patterns[g] << "\'\n";
    }
    if (nieh_speaks) cout << "\n";
    return (TRUE);
  }
  return (FALSE);
}/* end of tdf2xpodem */

void ATPG::tdf2xpodem_dtc(int &current_backtracks)
{
  int U_PO_idx = 0;
  int continuous_fail_count = 0;
  int fail_continuous_limit = 2;
  int select_fault_try = 10;
  for (fptr fptr_ele : flist_undetect)
  {
    fptr_ele->tried_dtc = false;
  }
  /*if (ncktin <= 32)
  {
    select_fault_try = 15;
  }*/
  while (continuous_fail_count < fail_continuous_limit)
  {
    // terminating condition: 1. no unknown PO,
    // 2. check unknown PO still exist
    while (U_PO_idx < cktout.size())
    {
      if (cktout[U_PO_idx]->value == U)
      {
        break;
      }
      U_PO_idx++;
    }
    if (U_PO_idx == cktout.size())
    {
      return;
    }
    fptr f_secondary = nullptr;
    // backtrace from unknown PO!
    wptr unknown_PO = cktout[U_PO_idx];
    queue<wptr> q_wire;
    queue<fptr> q_fault;
    bool PO_filled = false;
    q_wire.push(unknown_PO);
    int sl = 0;
    while (!PO_filled)
    {
      while (q_fault.empty() && sl++ < select_fault_try)
      {
        if (q_wire.empty())
	{
	  break;
	}
	else
	{
	  for (wptr w : q_wire.front()->inode.front()->iwire)
	  {
	    if (w->value == U) q_wire.push(w);
	  }
	  if (!q_wire.front()->udflist.empty())
	  {
	    for (fptr f : q_wire.front()->udflist)
	    {
	      if (f->detect != REDUNDANT && f->tried_dtc != true) q_fault.push(f);
	    }
	  }
	  q_wire.pop();
	}
      }
      if (q_fault.empty() || (select_fault_try == 0)) break; // try next unknown PO
      f_secondary = q_fault.front();
      q_fault.pop();
      for (int i = 0; i < cktin.size(); ++i) sort_wlist[i]->set_changed();
      for (int i = cktin.size(); i < sort_wlist.size(); ++i) sort_wlist[i]->value = U;
      sim();
      switch (tdf2xpodem_secondary(f_secondary, current_backtracks))
      {
	case TRUE:
	  if (unknown_PO->value != U) PO_filled = true;
	  break;
	default:
          break;
      }
	f_secondary->tried_dtc = true;
    }
    if (unknown_PO->value == U)
    {
      U_PO_idx++;
      continuous_fail_count++;
    }
  }
}
int ATPG::tdf2xpodem_secondary(const fptr fault, int &current_backtracks) {
  return 0;
}

