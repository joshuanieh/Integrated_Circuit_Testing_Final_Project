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
  bool nieh = false;
  string vec;
  int i, ncktwire, ncktin;
  forward_list<wptr> decision_tree; // design_tree (a LIFO stack)
  wptr wfault;
  int attempt_num = 0;  // counts the number of pattern generated so far for the given fault
  display_fault(fault);
  forward_list<wptr> decision_tree_for_pattern_1; // design_tree (a LIFO stack)
  int no_of_backtracks_1;
  *fake_fault = *fault;
  fake_fault->fault_type ^= 1;

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
            if (sort_wlist[fault->to_swlist]->value == !(fake_fault->fault_type)) {
              //Patterns found
              fprintf(stdout, "T\'");
              for (i = 0; i < cktin.size(); i++) {
                switch (cktin[i]->value) {
                  case 0:
                    fprintf(stdout, "0");
                    break;
                  case 1:
                    fprintf(stdout, "1");
                    break;
                  case U:
                    fprintf(stdout, "x");
                    break;
                  case D:
                    fprintf(stdout, "1");
                    break;
                  case D_bar:
                    fprintf(stdout, "0");
                    break;
                }
              }
              cout << " " << vec[0];
              fprintf(stdout, "'\n");
              find_test = true; // if fault effect reaches PO, done. Fig 7.10
              attempt_num++;
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
                if (nieh) cout << 50 << endl;//////////////////////////////////////////
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
                    fprintf(stdout, "T\'");
                    for (i = 0; i < cktin.size(); i++) {
                        switch (cktin[i]->value) {
                        case 0:
                            fprintf(stdout, "0");
                            break;
                        case 1:
                            fprintf(stdout, "1");
                            break;
                        case U:
                            fprintf(stdout, "x");
                            break;
                        case D:
                            fprintf(stdout, "1");
                            break;
                        case D_bar:
                            fprintf(stdout, "0");
                            break;
                        }
                    }
                    cout << " " << vec[0];
                    fprintf(stdout, "'\n");
                    find_test = true; // if fault effect reaches PO, done. Fig 7.10
                    attempt_num++;
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
         !(find_test && (attempt_num == total_attempt_num))) {

    /* check if test possible.   Fig. 7.1 */
    if (wpit = test_possible(fault)) {
      wpit->set_changed();
      /* insert a new PI into decision_tree */
      decision_tree.push_front(wpit);
    } else { // no test possible using this assignment, backtrack.
backtrack:
      if (nieh) cout << 7 << endl;//////////////////////////////////////////

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
        if (nieh) cout << 1 << endl;//////////////////////////////////////////
        // for (i=0; i<cktin.size();i++) {
        //         cout << i << " : " << cktin[i]->value << endl;
        //       }
        vec.clear();
        for (wptr w: cktin) {
            // cout << w->value << endl;
          vec.push_back(itoc(w->value));
          w->all_assigned = w->is_all_assigned();
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
            if (nieh) cout << 4 << endl;//////////////////////////////////////////
            sim();  // Fig 7.3
            if (sort_wlist[fault->to_swlist]->value == !(fake_fault->fault_type)) {
              //Patterns found
            //   for (i=0; i<cktin.size();i++) {
            //     cout << i << " : " << cktin[i]->value << endl;
            //   }
            //   cout << endl;
              fprintf(stdout, "T\'");
              for (i = 0; i < cktin.size(); i++) {
                switch (cktin[i]->value) {
                  case 0:
                    fprintf(stdout, "0");
                    break;
                  case 1:
                    fprintf(stdout, "1");
                    break;
                  case U:
                    fprintf(stdout, "x");
                    break;
                  case D:
                    fprintf(stdout, "1");
                    break;
                  case D_bar:
                    fprintf(stdout, "0");
                    break;
                }
              }
              cout << " " << vec[0];
              fprintf(stdout, "'\n");
              find_test = true; // if fault effect reaches PO, done. Fig 7.10
              attempt_num++;
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
              if (attempt_num != total_attempt_num) goto backtrack;
            }
            else {
              if (nieh) cout << 40 << endl;//////////////////////////////////////////
              no_of_backtracks_1 = 0;
              while ((no_of_backtracks_1 < backtrack_limit)) {
                /* check if test possible.   Fig. 7.1 */
                if (nieh) cout << 41 << endl;//////////////////////////////////////////
                if (wpit = test_possible(fake_fault)) {
                  if (nieh) cout << 42 << endl;//////////////////////////////////////////
                  wpit->remove_all_assigned();
                  if (nieh) cout << 42 << endl;//////////////////////////////////////////
                  wpit->set_changed();
                /* insert a new PI into decision_tree */
                  decision_tree_for_pattern_1.push_front(wpit);
                  if (nieh) cout << 43 << endl;//////////////////////////////////////////
                } else { // no test possible using this assignment, backtrack.
                  if (nieh) cout << 44 << endl;//////////////////////////////////////////
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
                  if (nieh) cout << 45 << endl;//////////////////////////////////////////
                  
                } // no test possible
                if (nieh) cout << 46 << endl;//////////////////////////////////////////

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
                  goto backtrack;
                } //try other pattern 2
                else { //if (wpit)
                // cout << wpit->name << endl;
                  sim();
                  if (sort_wlist[fault->to_swlist]->value == !(fake_fault->fault_type)) {
                    //Patterns found
                    fprintf(stdout, "T\'");
                    for (i = 0; i < cktin.size(); i++) {
                        switch (cktin[i]->value) {
                        case 0:
                            fprintf(stdout, "0");
                            break;
                        case 1:
                            fprintf(stdout, "1");
                            break;
                        case U:
                            fprintf(stdout, "x");
                            break;
                        case D:
                            fprintf(stdout, "1");
                            break;
                        case D_bar:
                            fprintf(stdout, "0");
                            break;
                        }
                    }
                    cout << " " << vec[0];
                    fprintf(stdout, "'\n");
                    find_test = true; // if fault effect reaches PO, done. Fig 7.10
                    attempt_num++;
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
                    if (attempt_num != total_attempt_num) goto backtrack;
                  }
                } // again
              } // while (one conditions)
            }
            if (nieh) cout << 5 << endl;//////////////////////////////////////////
        }
        else {
            if (nieh) cout << 6 << endl;//////////////////////////////////////////
            decision_tree_for_pattern_1.clear();
            if (nieh) cout << 9 << endl;//////////////////////////////////////////
            for (i = cktin.size(); i < ncktwire; i++) {
                sort_wlist[i]->value = U;
            }
            if (nieh) cout << vec << endl;//////////////////////////////////////////
            for (i = 0; i < cktin.size(); i++) {
                sort_wlist[i]->value = ctoi(vec[i]); //change back to original first pattern
                if (sort_wlist[i]->all_assigned) sort_wlist[i]->set_all_assigned();
                else sort_wlist[i]->remove_all_assigned();
            }
            if (nieh) cout << 11 << endl;//////////////////////////////////////////
            wpit = nullptr;
            goto backtrack;
        }
        if (nieh) cout << 3 << endl;//////////////////////////////////////////
      }  // if check_test()
    } // again
  } // while (three conditions)

  /* clear everything */
  for (wptr wptr_ele: decision_tree) {
    wptr_ele->remove_all_assigned();
  }
  decision_tree.clear();

  current_backtracks = no_of_backtracks;
  unmark_propagate_tree(fault->node);
  
  if (no_test) {
    fprintf(stdout,"redundant fault...\n\n");
    return (FALSE);
  } 
  else if (find_test) {
    /* normally, we want one pattern per fault */
    // if (total_attempt_num == 1) {

    //   for (i = 0; i < ncktin; i++) {
    //     switch (cktin[i]->value) {
    //       case 0:
    //       case 1:
    //         break;
    //       case D:
    //         cktin[i]->value = 1;
    //         break;
    //       case D_bar:
    //         cktin[i]->value = 0;
    //         break;
    //       case U:
    //         cktin[i]->value = rand() & 01;
    //         break; // random fill U
    //     }
    //   }
    //   display_io();
    // } else fprintf(stdout, "\n");  // do not random fill when multiple patterns per fault
    fprintf(stdout, "\n");
    return (TRUE);
  } else {
    fprintf(stdout,"test aborted due to backtrack limit...\n\n");
    return (MAYBE);
  }
}/* end of tdfpodem */
