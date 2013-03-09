import sys, re
from cStringIO import StringIO
from getopt import getopt, GetoptError

_verbose = False

def len_order(x, y):
    return len(x) - len(y)

def str_len_order(x, y):
    c = len(x) - len(y)
    return c if c != 0 else cmp(x, y)

def extract(io):
    val = io.getvalue()
    io.close()
    return val

def print_firstks(firsts, out):
    syms = list(firsts)
    syms.sort()

    for sym in syms:
        L = list(firsts[sym])
        L.sort(str_len_order)
        print >>out, sym, L

def add1(S0, prefixes, S1, n):
    for p in prefixes:
        if len(p) == n:
            S0.add(p)
        else:
            for s in S1:
                S0.add(p + s[0:n - len(p)])

def add_to_firsts(S, rhs, firsts, n):
    W = set([()])
    for sym in rhs:
        L = list(W)
        if not L:
            return False
        L.sort(len_order)
        W.clear()
        add1(W, L, firsts[sym], n)
    if W.issubset(S):
        return False
    else:
        S.update(W)
        return True

def indented_print(out, S, indent, width):
    posn = width
    for obj in S:
        rep = str(obj)
        if posn != indent and posn + len(rep) + 2 > width:
            out.write(" " * indent)
            posn = indent
        print >>out, rep + ",",
        posn += len(rep) + 2

class Common:
    def __str__(self):
        out = StringIO()
        self.write(out)
        return extract(out)

    def write(self, out):
        out.write(object.__repr__(self))

class Grammar(Common):
    def __init__(self):
        self.rules = []
        self.nonterms = set([])
        self.syms = set([])
        self._rule = None
        self._lhsMap = {}
        self._ruleMap = {}

    def open_rule(self, n, lhs):
        if lhs:
            self._lhsMap[lhs] = []
            self.syms.add(lhs)
            self.nonterms.add(lhs)
        else:
            lhs = self.rules[-1].lhs
        self._rule = Rule(n,lhs)
        self.rules.append(self._rule)
        self._lhsMap[lhs].append(self._rule)
        self._ruleMap[int(n)] = self._rule

    def add_rhs(self, sym):
        self._rule.add_rhs(sym)
        self.syms.add(sym)
        
    def is_term(self, sym):
        return sym in self.syms and sym not in self.nonterms

    def is_nonterm(self, sym):
        return sym in self.nonterms

    def rules_for(self, sym):
        return self._lhsMap[sym]

    def get_rule(self, n):
        return self._ruleMap[n]

    def write(self, out):
        for r in self.rules:
            r.write(out)
            print >>out

    def firstks(G, n):
        firsts = {}
        for sym in G.syms:
            if G.is_term(sym):
                firsts[sym] = set([(sym,)])
            else:
                firsts[sym] = set([])

        changed = True
        while changed:
            changed = False
            for rule in G.rules:
                changed |= add_to_firsts(firsts[rule.lhs], rule.rhs, firsts, n)
        return firsts


class Rule(Common):
    def __init__(self, id, lhs):
        self.id = int(id)
        self.lhs = lhs
        self.rhs = []

    def add_rhs(self, sym):
        self.rhs.append(sym)

    def get_sym(self, n):
        if n >= len(self.rhs):
            return None
        return self.rhs[n]

    def get_rhs_len(self):
        return len(self.rhs)

    def write(self, out):
        print >>out, self.lhs, ':',
        for sym in self.rhs:
            print >>out, sym,

class LR(Common):
    def __init__(self, grammar, N):
        self.G = grammar
        self.N = N
        self.states = {}
        self.state = None
        self.firsts = None
        
    def open_state(self, id):
        id = int(id)
        self.state = State(id, self)
        self.states[id] = self.state

    def add_LR0_item(self, n, p):
        self.state.add_LR0_item(int(n), p)

    def add_transition(self, sym, state_num):
        self.state.add_transition(sym, int(state_num))

    def add_conflict(self, rulenum):
        self.state.add_conflict(rulenum)

    def add_closures(self):
        for s in self.states:
            self.states[s].add_closure()

    def write(self, out):
        ids = list(self.states)
        ids.sort()
        for id in ids:
            self.states[id].write(out)
            print >>out

    def get_nextks(self, item, lookaheads, k=0):
        seqs = set([])
        add_to_firsts(seqs, item.rule.rhs[item.dot+k:], self.firsts, self.N)
        result = set([])
        add1(result, seqs, lookaheads, self.N)
        return result        

    def compute_lookaheads(self):
        init_state = self.states[0]
        for item in init_state.items:
            if item.dot == 0 and item.rule == self.G.rules[0]:
                init_state.lookaheads[item].add( (item.rule.rhs[-1],)
                                                 * (self.N-1) )
                break

        if not self.firsts:
            self.firsts = self.G.firstks(self.N)
            if _verbose:
                print_firstks (self.firsts, sys.stderr)

        self.states[0]._close_lookaheads()
        WQ = [self.states[0]]
        while WQ:
            WQ.pop()._propagate_lookaheads(WQ)

    def resolve_conflicts(self):
        for state in self.states:
            self.states[state].resolve_conflicts()
        
class State(Common):
    def __init__(self, id, machine):
        self.M = machine
        self.G = machine.G
        self.id = int(id)
        self.items = set([])
        self.transitions = {}
        self.conflicts = set([])
        self.lookaheads = {}
    
    def add_LR0_item(self, rule, p):
        rule = int(rule)
        item = Item(self.G.get_rule(rule), int(p))
        if item in self.items:
            return False
        self.items.add(item)
        self.lookaheads[item] = set([])
        return True

    def add_transition(self, sym, state_num):
        self.transitions[sym] = int(state_num)

    def add_conflict(self, rulenum):
        rulenum = int(rulenum)
        for item in self.items:
            if item.rule.id == rulenum and item.is_reduction ():
                self.conflicts.add(item)

    def resolve_conflicts(self):
        remaining_conflicts = set([])
        for item0 in self.conflicts:
            lookaheads0 = self.lookaheads[item0]
            for item1 in self.items:
                if item1 is item0:
                    continue
                lookaheads1 = self.lookaheads[item1]
                if item1.get_next() is None:
                    if lookaheads0 & lookaheads1:
                        remaining_conflicts.add(item0)
                        break
                elif lookaheads0 & self.M.get_nextks(item1, lookaheads1):
                    remaining_conflicts.add(item0)
                    break
                        
            else:
                if _verbose:
                    print >>sys.stderr, "Conflict on item", str(item0), \
                          "in state", self.id, "removed."
        self.conflicts = remaining_conflicts

    def is_conflicted(self):
        return bool(self.conflicts)

    def add_closure(self):
        changed = True
        while changed:
            changed = False
            for item in set(self.items):
                sym = item.get_next()
                if sym is not None and self.G.is_nonterm(sym):
                    for rule in self.G.rules_for(sym):
                        if self.add_LR0_item(rule.id, 0):
                            changed = True
                            
    def _close_lookaheads(self):
        result = False
        changed = True
        while changed:
            changed = False
            for item in self.items:
                sym = item.get_next()
                if sym and self.G.is_nonterm(sym):
                    firsts = self.M.get_nextks(item, self.lookaheads[item], 1)
                    for item1 in self.items:
                        if sym == item1.rule.lhs and item1.dot == 0:
                            lookaheads = self.lookaheads[item1]
                            if not firsts.issubset(lookaheads):
                                changed = result = True
                                lookaheads.update(firsts)
        
    def _push_lookaheads_from(self, unshifted_item, unshifted_lookaheads):
        if not unshifted_lookaheads:
            return False
        for item in self.items:
            if unshifted_item.shifts_to(item):
                lookaheads = self.lookaheads[item]
                if unshifted_lookaheads.issubset(lookaheads):
                    return False
                else:
                    self.lookaheads[item].update(unshifted_lookaheads)
                    return True
        raise RuntimeError, "did not find shifted item"

    def _propagate_lookaheads(self, WQ):
        changed = True
        for item in self.items:
            sym = item.get_next()
            if sym is not None and sym in self.transitions:
                next_state = self.M.states[self.transitions[sym]]
                changed = next_state._push_lookaheads_from(item,
                                                       self.lookaheads[item])
                if changed:
                    next_state._close_lookaheads()
                    WQ.append(next_state)

    def write(self, out):
        items = list(self.items)
        items.sort()
        print >>out, "state", self.id
        for item in items:
            if item in self.conflicts:
                out.write("* ")
            else:
                out.write("  ")
            item.write(out)
            if item in self.lookaheads:
                print >>out, "{"
                indented_print(out,self.lookaheads[item], 10, 65)
                print >>out, "}",
            print >>out
        trans = list(self.transitions)
        trans.sort()
        print >>out
        for sym in trans:
            if self.M.G.is_term(sym):
                print >>out, "    %-15s    shift, and go to state %d" \
                      % (sym, self.transitions[sym])
        print >>out
        for sym in trans:
            if self.M.G.is_nonterm(sym):
                print >>out, "    %-15s    go to state %d" \
                             % (sym, self.transitions[sym]) 

class Item(Common):
    def __init__(self, rule, p):
        self.rule = rule
        self.dot = p

    def get_next(self):
        return self.rule.get_sym(self.dot)

    def is_reduction(self):
        return self.dot == self.rule.get_rhs_len()

    def __hash__(self):
        return (self.rule.id << 6) + self.dot

    def __cmp__(self, x):
        c = self.rule.id - x.rule.id
        if c == 0:
            return self.dot - x.dot
        else:
            return c

    def write(self, out):
        print >>out, "%3d %s :" % (self.rule.id, self.rule.lhs),
        for k in xrange(self.rule.get_rhs_len()+1):
            if k == self.dot:
                print >>out, ".",
            sym = self.rule.get_sym(k)
            if sym:
                print >>out, sym,

    def shifts_to(self, x):
        return self.rule is x.rule and self.dot == x.dot-1

GRAMMAR_RULE_PATN = re.compile(r'\s+$|\s+(\d+)\s+(\S*)[|:](.*)')
SYMBOL_PATN = re.compile(r'/\* empty \*/|"(?:[^"\\]|\\.)*"|\S+')

def read_grammar(inp, G):
    while True:
        line = inp.readline()
        if line.strip() == "Grammar":
            break

    while True:
        line = inp.readline()
        mat = GRAMMAR_RULE_PATN.match(line)
        if not mat:
            break
        if mat.group(1) is not None:
            G.open_rule(mat.group(1), mat.group(2))
            for sym in SYMBOL_PATN.findall(mat.group(3)):
                if sym == "/* empty */":
                    continue
                G.add_rhs(sym)

    if not re.match(r'Terminals, with', line):
        raise SyntaxError, "grammar rules not properly terminated: " + line
    while not re.match(r'Nonterminals, with', line):
        line = inp.readline()
    inp.readline()
    while True:
        L = inp.readline()
        if re.match(r'\s*$', L):
            break
    

STATE_ITEM_PATN = \
    re.compile(r'state (\d+)'
               r'|\s+(\d+)\s+(?:\S+:|\|)\s*(.*)'
               r'|\s+(".*"|\S+)\s+(?:shift, and )?go to state (\d+)'
               r'|\s+(?:".*"|\S+)\s+\[reduce using rule (\d+)')

def find_dot(rhs):
    dot = 0
    for sym in SYMBOL_PATN.finditer(rhs):
        if sym.group(0) == ".":
            return dot
        dot += 1
    return None

def read_machine(inp, G, M):
    while True:
        L = inp.readline()
        if L == '':
            M.add_closures()
            break
        mat = STATE_ITEM_PATN.match(L)
        if not mat:
            continue
        if mat.group(1) is not None:
            M.open_state(mat.group(1))
        elif mat.group(2) is not None:
            M.add_LR0_item(mat.group(2), find_dot(mat.group(3)))
        elif mat.group(4) is not None:
            M.add_transition(mat.group(4), mat.group(5))
        else:
            M.add_conflict(int(mat.group(6)))

def Usage():
    print >>sys.stderr, 'Usage: python analysis.py [ --verbose ] (FILE | -) [N]'
    sys.exit(1)

def main(raw_args):
    global _verbose

    try:
        opts, args = getopt(raw_args, '', ['verbose'])
    except GetoptError:
        Usage()
    
    for opt, val in opts:
        if opt == '--verbose':
            _verbose = True
    if not (1 <= len(args) <= 2):
        Usage()
    if args[0] == '-':
        inp = sys.stdin
    else:
        inp = open(args[0])
    if len(args) > 1:
        N = int(args[1])
    else:
        N = 1

    G = Grammar()
    M = LR(G, N)
    read_grammar(inp, G)
    read_machine(inp, G, M)
    if _verbose:
        M.write(sys.stderr)
    M.compute_lookaheads()
    M.resolve_conflicts()
    M.write(sys.stdout)

if __name__ == '__main__':
    main(sys.argv[1:])
