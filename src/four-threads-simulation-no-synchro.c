#include "../inc/four-threads-simulation-no-synchro.h"

void dispatch(grid* field, std::list<int> exiting[], std::list<int> responsability[]) {
    std::list<int>::iterator iter;
    for (unsigned i = 0; i < 4; ++i) {
	for (iter = exiting[i].begin(); iter != exiting[i].end(); ++iter) {
	    int index = *iter;
	    person p = field->people[index];
	    int zone = (int) (p.origin_x < DEFAULT_GRID_WIDTH / 2)?
		((p.origin_y < DEFAULT_GRID_HEIGHT / 2)? TOP_LEFT : BOTTOM_LEFT) :
		((p.origin_y < DEFAULT_GRID_HEIGHT / 2)? TOP_RIGHT : BOTTOM_RIGHT);
	    responsability[zone].push_front(index);
	    exiting[i].erase(iter++);
	}
    }
}


void* four_threads_simulation(void* ptr_args)
{
    field_zone zone = ((thread_args*) ptr_args)->zone;
    grid* field = ((thread_args*) ptr_args)->field;
    std::list<int>* responsability = ((thread_args*) ptr_args)->responsability;
    std::list<int>* exiting = ((thread_args*) ptr_args)->exiting;
    std::list<int>::iterator iter;

    int xmin, xmax, ymin, ymax;
    
    switch(zone) {
    case TOP_LEFT:
	xmin = 0; xmax = (DEFAULT_GRID_WIDTH / 2) - 1;
	ymin = 0; ymax = (DEFAULT_GRID_HEIGHT / 2) - 1;
	break;
    case TOP_RIGHT:
	xmin = (DEFAULT_GRID_WIDTH / 2); xmax =  DEFAULT_GRID_WIDTH - 1;
	ymin = 0; ymax = (DEFAULT_GRID_HEIGHT / 2) - 1;
	break;
    case BOTTOM_LEFT:
	xmin = 0; xmax = (DEFAULT_GRID_WIDTH / 2) - 1;
	ymin = (DEFAULT_GRID_HEIGHT / 2) - 1; ymax = DEFAULT_GRID_HEIGHT - 1;
	break;
    case BOTTOM_RIGHT:
	xmin = (DEFAULT_GRID_WIDTH / 2); xmax =  DEFAULT_GRID_WIDTH - 1;
	ymin = (DEFAULT_GRID_HEIGHT / 2) - 1; ymax = DEFAULT_GRID_HEIGHT - 1;
	break;
    }
    
    for (unsigned p = 0; p < field->person_count; ++p) {
	field_zone person_zone = (::field_zone) (field->people[p].origin_x < DEFAULT_GRID_WIDTH / 2)?
	    ((field->people[p].origin_y < DEFAULT_GRID_HEIGHT / 2)? TOP_LEFT : BOTTOM_LEFT) :
	    ((field->people[p].origin_y < DEFAULT_GRID_HEIGHT / 2)? TOP_RIGHT : BOTTOM_RIGHT);
	
	if (zone == person_zone) {
	    responsability->push_back(p);
	}
    }

    while (! is_finished(field)) {
	for (iter = responsability->begin(); iter != responsability->end(); ++iter) {
	    int p = *iter;
	    
	    if (field->people[p].status == IN) {
		double min_dist = distance_to_azimuth(field->people[p].origin_x,
						      field->people[p].origin_y);

		direction dir = UNKNOWN_DIR;
		
		for (int m = 0; m < NB_MOVES; ++m) {
		    int x = field->people[p].origin_x + moves[m].x;
		    int y = field->people[p].origin_y + moves[m].y;

		    if (can_move(field, field->people[p], (::direction) m)) {
			float dist = distance_to_azimuth(x, y);
			
			if (min_dist == 0 || dist < min_dist) {
			    min_dist = dist;
			    dir = (::direction) m;
			}
		    }
		}

		if (dir == UNKNOWN_DIR)
		    continue;
				
		move_person(field, p, dir);

		if (field->people[p].origin_x == 0) {
		    field->people[p].status = OUT;
		    delete_person(field, p);
		} else {
		    if (field->people[p].origin_x < xmin || field->people[p].origin_x > xmax ||
			field->people[p].origin_y < ymin || field->people[p].origin_y > ymax) {
			responsability->erase(iter++);
			exiting->push_front(p);
		    }
			    
		}
	    }
	}

	
	
	#ifdef GUI
	  SDL_Delay(10);
	#endif
    }

    return NULL;
}
