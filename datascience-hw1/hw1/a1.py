#!/usr/bin/python
"""
CS194-16 Data Science Assignment 1
UC Berkeley

Sean Wiser
swiser@berkeley.edu

"""

from __future__ import division
import math
import sys

import numpy as np
from pandas import *


def add_party_column(df):
    """Add a column representing the candidate's party to the data frame.

    Args:
        df: A DataFrame generated from the campaign finance data csv file.

    Returns:
        A DataFrame based on df with an additional column keyed "party" whose
        values represent the party of the candidates.
        For Democratic candidates, use value "Democrat".
        For Republicans, use "Republican".
        For Libertarian candidates, use value "Libertarian".
    """
    df['party'] = df['cand_nm']

    for index in xrange(df.index.size):
        cand_nm = df['cand_nm'][index]
        if cand_nm == "Obama, Barack":
            pol_party = "Democrat"
        elif cand_nm == "Paul, Ron" or cand_nm == "Johnson, Gary Earl":
            pol_party = "Libertarian"
        else:
            pol_party = "Republican"
            
        df['party'][index] = pol_party
        #print df['cand_nm'][index] + " " + df['party'][index] + " \n"   
    
    return df     
def make_csv_don_months(df):
    df["contb_month"] = df["cand_nm"]
    for index in xrange(df.index.size):
        df["contb_month"][index] = df['contb_receipt_dt'][index][3:6]
       
    month_don_dict = {"JAN": 0, "FEB": 0, "MAR": 0, "APR": 0, "MAY": 0,
                      "JUN": 0, "JUL": 0, "AUG": 0, "SEP": 0,
                      "OCT": 0, "NOV": 0, "DEC": 0}

    for month in month_don_dict.keys():
        month_don_dict[month] = num_donation_records(df[df["contb_month"].isin([month])])
    
    s = Series(month_don_dict)
    s.to_csv(path="month_density.csv");


        
       
def num_donation_records(df):
    """Return the total number of donation records.
    
    Args:
        df: A DataFrame generated from the campaign finance data csv file.

    Returns:
        An integer count of the number of donation records.
        
        ASSUMING donation_amount > 0
    """
    def pos_contr(amt):
        if amt > 0:
            return 1
        else:
            return 0
        
    df_amt = df['contb_receipt_amt'].map(pos_contr)
    return df_amt.sum()
    
def num_refund_records(df):
    """Return the number of refund records.
    
    Args:
        df: A DataFrame generated from the campaign finance data csv file.

    Returns:
        An integer count of the number of refund records.
    """
    def pos_contr(amt):
        if amt < 0:
            return 1
        else:
            return 0
        
    df_amt = df['contb_receipt_amt'].map(pos_contr)
    return df_amt.sum()


def num_donors(df, state):
    """Return the number of people that have donated in the given state.
    
    Assume people have unique names (i.e. no two person share the same name).
    Do not count the same person twice.

    Args:
        df: A DataFrame generated from the campaign finance data csv file.
        state: The state of interest in capitalized two-letter format,
            e.g. "CA".

    Returns:
        An integer count of the number of donors.
    """

    df = df[df['contbr_st'].isin([state])]
    df = df[df['contb_receipt_amt'] > 0]
    return df.index.size
  

def top_candidate_by_num_donors(df, state):
    """Find the candidate that received the most donations (by the number of
    donation records) in a given state.

    Args:
        df: A DataFrame generated from the campaign finance data csv file.
        state: The state of interest in capitalized two-letter format,
            e.g. "CA".

    Returns:
        A tuple consisting of a pair of values. The first value should be the
        name of the candidate, and the second value should be the fraction of
        the number of donations he received compared with all candidates.
        E.g. ('Cain, Herman', 0.115).
    """
    def pos_contr(amt):
        if amt > 0:
            return 1
        else:
            return 0
    """
    #Filter out all negative donations
    #All donations are 1 if they are positive, 0 if not
    """
    s_amt = df['contb_receipt_amt'].map(pos_contr)
    
    """
    #Add candidate name and contribution state
    #This sets up a DataFrame in a way to reuse top_candidate_by_amount
    """
    df_result = DataFrame(s_amt)
    df_result['contbr_st'] = df['contbr_st']
    df_result['cand_nm'] = df['cand_nm']

    return top_candidate_by_amount(df_result, state)
   
    
        
def top_candidate_by_amount(df, state):
    """Find the candidate that received the highest amount of donations in
    a given state.

    Args:
        df: A DataFrame generated from the campaign finance data csv file.
        state: The state of interest in capitalized two-letter format,
            e.g. "CA".

    Returns:
        A tuple consisting of a pair of values. The first value should be the
        name of the candidate, and the second value should be the fraction of
        donations he received compared with all candidates.
        E.g. ('Cain, Herman', 0.05).
    """
      
    """
    Finds the candidate who raised the most money and the amount they raised
    """
    def find_most_donations(dic):
        max = -1
        candidate = 'Null'
        for cand in dic:
            if dic[cand] > max:
                max = dic[cand]
                candidate = cand
        return candidate, max
    
    """
    Finds amt fundraised
    """
    def find_total_donations(dic):
        total = 1
        for cand in dic:
            total = total + dic[cand]
        return total

    """
    Keep a dictionary with the key as the candidate name and the value is the cumsum
    iterate through
    """
    dict_donations = {}
    df = df[df["contbr_st"].isin([state])]
    df = df[df["contb_receipt_amt"] > 0]
    
    dict_donations = {}
    for row in df.iterrows():
        row_info = row[1]
        cand_nm = row_info['cand_nm']
        contb_receipt_amt = row_info['contb_receipt_amt']
        if cand_nm not in dict_donations:
            dict_donations[cand_nm] = contb_receipt_amt
        else:
            dict_donations[cand_nm] = dict_donations[cand_nm] + contb_receipt_amt
  
    richest_candidate, most_fundraised = find_most_donations(dict_donations)
    total_fundraised = find_total_donations(dict_donations)
    
    return (richest_candidate, most_fundraised/total_fundraised)


def top_party_by_num_donors(df, state):
    """Find the party that received the most donations (by the number of
    donation records) in a given state.

    Args:
        df: A DataFrame generated from the campaign finance data csv file
            with the column "party" added.
        state: The state of interest in capitalized two-letter format,
            e.g. "CA".

    Returns:
        A tuple consisting of a pair of values. The first value should be the
        name of the party, as defined in add_party_column(df), and the second
        value should be the fraction of the number of donations the party
        received compared with other parties. E.g. ('Democrat', 0.115).
    """

    df = add_party_column(df)
    df_st = df[df['contbr_st'].isin([state])]
    df_st = df_st[df_st['contb_receipt_amt']> 0]
    df_dem = df_st[df_st['party'].isin(['Democrat'])]
    df_gop = df_st[df_st['party'].isin(['Republican'])]
    df_lib = df_st[df_st['party'].isin(['Libertarian'])]

    dem_don = df_dem.index.size#num_donors(df_dem, state)
    gop_don = df_gop.index.size#num_donors(df_gop, state)
    lib_don = df_lib.index.size#num_donors(df_lib, state)
    tot_don = df_st.index.size#num_donors(df_st, state)

    party_winner = ""
    winning_amount = 1
    if dem_don > gop_don and dem_don > lib_don:
        party_winner = "Democrat"
        winning_amount = dem_don
    elif gop_don > dem_don and gop_don > lib_don:
        party_winner = "Republican"
        winning_amount = gop_don
    else:
        party_winner = "Libertarian"
        winning_amount = lib_don
    
    return (party_winner, winning_amount/(1+tot_don) )
      
    


def top_party_by_amount(df, state):
    """Find the party that received the highest amount of donations in a given
    state.

    Args:
        df: A DataFrame generated from the campaign finance data csv file
            with the column "party" added.
        state: The state of interest in capitalized two-letter format,
            e.g. "CA".

    Returns:
        Return a tuple consisting of a pair of values. The first value should be
        the name of the party, as defined in add_party_column(df), and the
        second value should be the fraction of the amount of donations the party
        received compared with other parties. E.g. ('Democrat', 0.21).
    """
    
    """
    add the party column and set up the DataFrame for recycling code
    set the cand_nm = party so top_candidate_by_amount will find the top party by amount
    """
    df = add_party_column(df)
    df['cand_nm'] = df['party']
    return top_candidate_by_amount(df, state)


def num_bipartisan_donors(df):
    """Find the number of people that have donated to more than one parties.
    
    Args:
        df: A DataFrame generated from the campaign finance data csv file
            with the column "party" added.

    Returns:
        An integer count of the number of people that have donated to more than
        one parties.
    """
    df = add_party_column(df)
    set_dem_don = set()
    set_gop_don = set()
    set_lib_don = set()
    
    """
    Put all the donaters in a party sets
    """
    for row in df.iterrows():
        row_info = row[1]
        party = row_info['party']
        contbr_name = row_info['contbr_nm']
        if party == 'Democrat':
            set_dem_don.add(contbr_name)
        elif party == 'Republican':
            set_gop_don.add(contbr_name)
        else:
            set_lib_don.add(contbr_name)
    
    
    """
    Return the pairwise intersections - 2triIntersection by the inclusion exclusion principle
    """
    return len(set_dem_don.intersection(set_gop_don)) + len(set_dem_don.intersection(set_lib_don)) + len(set_gop_don.intersection(set_lib_don)) - 2 * len(set_gop_don.intersection(set_lib_don, set_dem_don))

def make_bucket_csv(df):
    buckets = bucketize_donation_amounts(df)
    
    buckets_dict = {"0-1": buckets[0], "1-10": buckets[1], "10-100": buckets[2],
                    "100-1000": buckets[3], "1000-10000": buckets[4], 
                    "10000-100000": buckets[5], "100000-1000000": buckets[6],
                    "1000000-10000000": buckets[7]}
    s = Series(buckets_dict)
    s.to_csv(path="bucket_histogram.csv");
    
    
def bucketize_donation_amounts(df):
    """Generate a histogram for the donation amount.

    Put donation amount into buckets and generate a histogram for these buckets.
    The buckets are: (0, 1], (1, 10], (10, 100], (100, 1000], (1000, 10000],
    (10000, 100000], (100000, 1000000], (1000000, 10000000].

    Args:
        df: A DataFrame generated from the campaign finance data csv file.

    Returns:
        A list containing 8 integers. The Nth integer is the count of donation
        amounts that fall into the Nth bucket. E.g. [2, 3, 4, 5, 4, 3, 1, 1].
    """
    
    """
    Range_finder is a function that takes a high and low
    It returns a function that checks if the amount passed in from DF is in the bounds
    If it does, it receives a one, if not it receives a 0
    THis way one can just sum() the series quickly to see how many donators gave inbetween those bounds
    """
    def range_finder(low, high):
        def range_filter(amt):
            if amt <= high and amt > low:
                return 1
            else:
                return 0
        return range_filter
    
    list_buckets = []
    
    #get all the buckets    
    bucket1 = df['contb_receipt_amt'].map(range_finder(0, 1))
    bucket2 = df['contb_receipt_amt'].map(range_finder(1, 10))
    bucket3 = df['contb_receipt_amt'].map(range_finder(10, 100))
    bucket4 = df['contb_receipt_amt'].map(range_finder(100, 1000))
    bucket5 = df['contb_receipt_amt'].map(range_finder(1000, 10000))
    bucket6 = df['contb_receipt_amt'].map(range_finder(10000, 100000))
    bucket7 = df['contb_receipt_amt'].map(range_finder(100000, 1000000))
    bucket8 = df['contb_receipt_amt'].map(range_finder(1000000, 10000000))
    
    #add the sums to the list
    list_buckets.append(bucket1.sum())
    list_buckets.append(bucket2.sum())
    list_buckets.append(bucket3.sum())
    list_buckets.append(bucket4.sum())
    list_buckets.append(bucket5.sum())
    list_buckets.append(bucket6.sum())
    list_buckets.append(bucket7.sum())
    list_buckets.append(bucket8.sum())
           
    return list_buckets 
  
def make_st_donor_csv(df):
    dict_states = {}
    for row in df.iterrows():
        row_info = row[1]
        state = row_info['contbr_st']
        dict_states[state] = 1
    for state in dict_states.keys():
        dict_states[state] = num_donors(df, state)
    
    s = Series(dict_states)
    s.to_csv(path="statedon.csv")
def make_donor_csv(df):
    df = df[df["contbr_st"].isin(["CA"])]
    df = df[df["contb_receipt_amt"] < 100000]
    df = df[df["contb_receipt_amt"] > 0]
    s = df["contb_receipt_amt"]
    s.to_csv(path="donor_den.csv")
    
def make_candidate_amt_csv(df):
    """
    Keep a dictionary with the key as the candidate name and the value is the cumsum
    iterate through
    """
    dict_states = {}
    for row in df.iterrows():
        row_info = row[1]
        state = row_info['contbr_st']
        dict_states[state] = 1
    
    dict_donations = {}
    for state in dict_states.keys():
        df2 = df[df["contbr_st"].isin([state])]
        df2 = df[df["contb_receipt_amt"] > 0]
        for row in df.iterrows():
            row_info = row[1]
            cand_nm = row_info['cand_nm']
            contb_receipt_amt = row_info['contb_receipt_amt']
            if cand_nm not in dict_donations:
                dict_donations[cand_nm] = contb_receipt_amt
            else:
                dict_donations[cand_nm] = dict_donations[cand_nm] + contb_receipt_amt   
    s = Series(dict_donations)
    s.to_csv(path="canddon.csv") 
def make_party_amt(df):
    total = num_donation_records(df)
    add_party_column(df)
    dfd = df[df["party"].isin(["Democrat"])]
    dfr = df[df["party"].isin(["Republican"])]
    dfl = df[df["party"].isin(["Libertarian"])]
    
    dict_party = {"Democrat": num_donation_records(dfd)/total,
                  "Republican": num_donation_records(dfr)/total,
                  "Libertarian": num_donation_records(dfl)/total}
    
    s = Series(dict_party)
    s.to_csv(path="party_pie.csv")
def main(*args):
    df = read_csv('P00000001-ALL.txt')
    
    #TESTS
    
    #print num_donation_records(df)
    #print num_refund_records(df)
    #print num_donors(df, 'CA')
    #print top_candidate_by_num_donors(df, 'TX')
    #print top_candidate_by_amount(df, 'CA')
    #print top_party_by_num_donors(df, 'TX')
    #print top_party_by_amount(df, 'CA')
    #print num_bipartisan_donors(df)
    #print bucketize_donation_amounts(df)
    #make_bucket_csv(df)
    #make_csv_don_months(df)
    #make_romney_bucket_csv(df)
    #make_obama_bucket_csv(df)
    #make_donor_csv(df)
    #make_candidate_amt_csv(df)
    make_party_amt(df)
    
    #Find same names
    """
    s =df['contbr_nm'].sort_index()
    print s[:50]
    """
    
    #Find how many states obama is on top
    #find how many state republicans are on top

    #Find states
    """
    dict_states = {}
    for row in df.iterrows():
        row_info = row[1]
        state = row_info['contbr_st']
        dict_states[state] = 1
        
    #find if obama was on top: 61
    #obama_win_counter = 0
    gop_win_counter = 0
    for state in dict_states.keys():
        #if top_candidate_by_amount(df,state)[0] == "Obama, Barack":
        if top_party_by_num_donors(df, state)[0] == 'Republican':
             #obama_win_counter = obama_win_counter + 1
             gop_win_counter = gop_win_counter +1
    #print obama_win_counter
    print gop_win_counter
"""
    


if __name__ =='__main__':
    sys.exit(main(*sys.argv[1:]))

