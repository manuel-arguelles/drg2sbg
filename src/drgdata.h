/* 
 * Copyright (C) 2012  Manuel Argüelles <manuel.arguelles@gmail.com>
 *
 * This file is part of drg2sbg.
 *
 * Drg2sbg is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Foobar is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef DRG_DATA_H
#define DRG_DATA_H


enum drg_elements {
    HEADER = 0,
    TITLE,
    IMAGE,
    INFO,
    SBG_DATA,
    MAX_ELEMENTS
};

typedef struct drgdata_ DrgData; 


DrgData *drg_data_new(void);

void drg_data_free(DrgData *drg);

void drg_add_byte(DrgData *drg, int element, int byte);

unsigned char *drg_get_uncoded_data(DrgData *drg, int element, size_t *len);

#endif /* DRG_DATA_H */
