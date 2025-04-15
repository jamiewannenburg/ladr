import ladr.mace4 as mace4
from ladr.formula import Formula,term_to_formula, formula_to_term
from ladr.parse import parse_term_from_string #a.k.a read_term
from ladr.term import Term, copy_term, build_binary_term, build_unary_term, term0
import ladr.top_input as top_input

class Mace4:
    def __init__(self):
        self.options = mace4.MaceOptions()
        mace4.init_mace_options(self.options)
        self.stats = mace4.MaceStats()
        self.results = None

    def set_options(self, **kwargs):
        for key, value in kwargs.items():
            if hasattr(self.options, key):
                setattr(self.options, key, value)

    def run(self, clauses):
        self.results = mace4.mace4(clauses, self.options)
        return self.results

    def get_results(self):
        return self.results

    def get_stats(self):
        return self.stats

    def exit(self, exit_code):
        mace4.mace4_exit(exit_code)

    def read_input(self, input_content: str) -> None:
        """
        Read and process a Mace4/Prover9 input file content as a string.
        Populates class variables with goals, hints, distinct items, formulas, and terms.

        Args:
            input_content (str): The content of a Mace4/Prover9 input file as a string.
        """
        # Initialize class variables to store parsed input
        self.goals = []
        self.hints = []
        self.distinct = []
        self.formulas = []
        self.terms = []

        # Split input content into lines for processing
        lines = input_content.splitlines()
        current_line = 0
        echo = False  # Whether to echo input (as in read_from_file)
        unknown_action = 1  # Default action for unknown commands (from read_from_file)

        while current_line < len(lines):
            line = lines[current_line].strip()
            current_line += 1

            if not line or line.startswith('#'):
                continue  # Skip empty lines and comments

            # Check for commands like 'set' and 'clear' (from read_from_file)
            if line.startswith('set('):
                self._process_set_command(line)
            elif line.startswith('clear('):
                self._process_clear_command(line)
            elif line.startswith('formulas('):
                label = self._extract_label(line, 'formulas')
                formula_list = self._read_list(lines, current_line, 'formulas')
                current_line += len(formula_list) + 1  # Skip past the list and 'end_of_list.'
                if label == 'goals':
                    self.goals.extend(formula_list)
                elif label == 'hints':
                    self.hints.extend(formula_list)
                else:
                    self.formulas.extend(formula_list)
            elif line.startswith('terms('):
                label = self._extract_label(line, 'terms')
                term_list = self._read_list(lines, current_line, 'terms')
                current_line += len(term_list) + 1  # Skip past the list and 'end_of_list.'
                if label == 'distinct':
                    self.distinct.extend(term_list)
                else:
                    self.terms.extend(term_list)
            else:
                # Handle unknown or unsupported commands
                if unknown_action == 1:
                    print(f"Warning: Unknown command at line {current_line}: {line}")
                # Otherwise, ignore or fatal error based on unknown_action

        # Process distinct terms into formulas (as in process_distinct_terms)
        distinct_forms = self._process_distinct_terms(self.distinct)
        self.formulas.extend(distinct_forms)

        # TODO: Further processing like embedding into topforms and clausification
        # This will require binding to C functions like embed_formulas_in_topforms and process_input_formulas

    def _read_list(self, lines: list, start_line: int, list_type: str) -> list:
        """
        Read a list of formulas or terms from the input lines starting at start_line until 'end_of_list.'

        Args:
            lines (list): List of input lines.
            start_line (int): Starting line index to read from.
            list_type (str): Type of list ('formulas' or 'terms').

        Returns:
            list: List of items read from the input.
        """
        items = []
        current_line = start_line
        while current_line < len(lines):
            line = lines[current_line].strip()
            if line == 'end_of_list.':
                break
            if line and not line.startswith('#'):
                items.append(line)
            current_line += 1
        return items

    def _extract_label(self, line: str, command: str) -> str:
        """
        Extract the label from a command like 'formulas(label)' or 'terms(label)'.

        Args:
            line (str): The command line.
            command (str): The command type ('formulas' or 'terms').

        Returns:
            str: The extracted label or empty string if none.
        """
        start = line.find(command + '(') + len(command) + 1
        end = line.find(')', start)
        if start < end:
            return line[start:end].strip()
        return ''

    def _process_set_command(self, line: str) -> None:
        """
        Process a 'set' command to update options.

        Args:
            line (str): The set command line.
        """
        # Extract the flag or option from set(flag)
        flag = line[4:-1].strip()  # Remove 'set(' and ')'
        if hasattr(self.options, flag):
            setattr(self.options, flag, True)
        # TODO: Handle more complex set commands with parameters

    def _process_clear_command(self, line: str) -> None:
        """
        Process a 'clear' command to update options.

        Args:
            line (str): The clear command line.
        """
        # Extract the flag or option from clear(flag)
        flag = line[6:-1].strip()  # Remove 'clear(' and ')'
        if hasattr(self.options, flag):
            setattr(self.options, flag, False)
        # TODO: Handle more complex clear commands

    def _embed_formulas_in_topforms(self, formulas: list, is_input: bool) -> list:
        """
        Convert a list of formulas into Topform structures by calling the C function.
        Mirrors embed_formulas_in_topforms from top_input.h.

        Args:
            formulas (list): List of formulas to embed (should be Formula objects compatible with LADR bindings).
            is_input (bool): Whether these are input formulas (affects processing).

        Returns:
            list: List of Topform objects created from the formulas.
        """
        return top_input.embed_formulas_in_topforms(formulas, is_input)

    def _process_input_formulas(self, formulas: list, echo: bool) -> list:
        """
        Dummy function to simulate processing input formulas (e.g., clausification).
        Mirrors process_input_formulas from clausify.h or related files.

        Args:
            formulas (list): List of formulas to process.
            echo (bool): Whether to echo the processing steps.

        Returns:
            list: List of processed formulas or clauses.
        """
        # TODO: Bind to C function process_input_formulas
        return formulas

    def _process_goal_formulas(self, goals: list, echo: bool) -> list:
        """
        Dummy function to simulate processing goal formulas (includes negation).
        Mirrors process_goal_formulas from clausify.h or related files.

        Args:
            goals (list): List of goal formulas to process.
            echo (bool): Whether to echo the processing steps.

        Returns:
            list: List of processed and negated goal formulas or clauses.
        """
        # TODO: Bind to C function process_goal_formulas
        return goals

    def _parse_term(self, term_str: str) -> object:
        """
        Dummy function to simulate parsing a term string into a Term structure.
        Mirrors parse_term_from_string or related functions in the C code.

        Args:
            term_str (str): The string representation of a term.

        Returns:
            object: Placeholder for a Term structure.
        """
        # TODO: Implement term parsing or bind to C function
        return term_str

    def _parse_formula(self, formula_str: str) -> object:
        """
        Dummy function to simulate parsing a formula string into a Formula structure.
        Mirrors parse_formula_from_string or related functions in the C code.

        Args:
            formula_str (str): The string representation of a formula.

        Returns:
            object: Placeholder for a Formula structure.
        """
        # TODO: Implement formula parsing or bind to C function
        return formula_str

    def _clausify_formula(self, formula: object) -> list:
        """
        Dummy function to simulate converting a formula into clauses.
        Mirrors clausification processes in the C code.

        Args:
            formula (object): The formula to clausify.

        Returns:
            list: List of clauses derived from the formula (placeholder).
        """
        # TODO: Implement clausification or bind to C function
        return [formula]

    def _process_distinct_terms(self, distinct_lists: list) -> list:
        """
        Process distinct terms into formulas, creating inequalities between pairs of terms.
        Mirrors process_distinct_terms from mace4.c.

        Args:
            distinct_lists (list): List of distinct terms to process.

        Returns:
            list: List of processed formulas derived from distinct terms (inequalities).
        """
        processed = []
        if not distinct_lists:
            return processed

        # Parse each term string into a Term object if it's a string
        terms = []
        for term_str in distinct_lists:
            if isinstance(term_str, str):
                term = parse_term_from_string(term_str)
                terms.append(term)
            else:
                terms.append(term_str)  # Already a Term object or similar

        # Create inequality formulas for each pair of terms
        for i in range(len(terms)):
            for j in range(i + 1, len(terms)):
                term1 = terms[i]
                term2 = terms[j]
                # Build an inequality formula: term1 != term2
                inequality_term = build_binary_term('!=', copy_term(term1), copy_term(term2))
                inequality_formula = term_to_formula(inequality_term)
                processed.append(inequality_formula)

        return processed
