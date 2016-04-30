/*
 *  Copyright 2004-2006 Adrian Thurston <thurston@complang.org>
 *            2004 Erich Ocean <eric.ocean@ampede.com>
 *            2005 Alan West <alan@alanz.com>
 */

/*  This file is part of Ragel.
 *
 *  Ragel is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 * 
 *  Ragel is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 * 
 *  You should have received a copy of the GNU General Public License
 *  along with Ragel; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 */

#include "ragel.h"
#include "mlfflat.h"
#include "redfsm.h"
#include "gendata.h"

std::ostream &OCamlFFlatCodeGen::TO_STATE_ACTION( RedStateAp *state )
{
	int act = 0;
	if ( state->toStateAction != 0 )
		act = state->toStateAction->actListId+1;
	out << act;
	return out;
}

std::ostream &OCamlFFlatCodeGen::FROM_STATE_ACTION( RedStateAp *state )
{
	int act = 0;
	if ( state->fromStateAction != 0 )
		act = state->fromStateAction->actListId+1;
	out << act;
	return out;
}

std::ostream &OCamlFFlatCodeGen::EOF_ACTION( RedStateAp *state )
{
	int act = 0;
	if ( state->eofAction != 0 )
		act = state->eofAction->actListId+1;
	out << act;
	return out;
}

/* Write out the function for a transition. */
std::ostream &OCamlFFlatCodeGen::TRANS_ACTION( RedTransAp *trans )
{
	int action = 0;
	if ( trans->action != 0 )
		action = trans->action->actListId+1;
	out << action;
	return out;
}

/* Write out the function switch. This switch is keyed on the values
 * of the func index. */
std::ostream &OCamlFFlatCodeGen::TO_STATE_ACTION_SWITCH()
{
	/* Loop the actions. */
	for ( GenActionTableMap::Iter redAct = redFsm->actionMap; redAct.lte(); redAct++ ) {
		if ( redAct->numToStateRefs > 0 ) {
			/* Write the entry label. */
			out << "\t| " << redAct->actListId+1 << " ->\n";

			/* Write each action in the list of action items. */
			for ( GenActionTable::Iter item = redAct->key; item.lte(); item++ )
				ACTION( out, item->value, 0, false );

			out << "\t()\n";
		}
	}

	genLineDirective( out );
	return out;
}

/* Write out the function switch. This switch is keyed on the values
 * of the func index. */
std::ostream &OCamlFFlatCodeGen::FROM_STATE_ACTION_SWITCH()
{
	/* Loop the actions. */
	for ( GenActionTableMap::Iter redAct = redFsm->actionMap; redAct.lte(); redAct++ ) {
		if ( redAct->numFromStateRefs > 0 ) {
			/* Write the entry label. */
			out << "\t| " << redAct->actListId+1 << " ->\n";

			/* Write each action in the list of action items. */
			for ( GenActionTable::Iter item = redAct->key; item.lte(); item++ )
				ACTION( out, item->value, 0, false );

			out << "\t()\n";
		}
	}

	genLineDirective( out );
	return out;
}

std::ostream &OCamlFFlatCodeGen::EOF_ACTION_SWITCH()
{
	/* Loop the actions. */
	for ( GenActionTableMap::Iter redAct = redFsm->actionMap; redAct.lte(); redAct++ ) {
		if ( redAct->numEofRefs > 0 ) {
			/* Write the entry label. */
			out << "\t| " << redAct->actListId+1 << " ->\n";

			/* Write each action in the list of action items. */
			for ( GenActionTable::Iter item = redAct->key; item.lte(); item++ )
				ACTION( out, item->value, 0, true );

			out << "\t()\n";
		}
	}

	genLineDirective( out );
	return out;
}

/* Write out the function switch. This switch is keyed on the values
 * of the func index. */
std::ostream &OCamlFFlatCodeGen::ACTION_SWITCH()
{
	/* Loop the actions. */
	for ( GenActionTableMap::Iter redAct = redFsm->actionMap; redAct.lte(); redAct++ ) {
		if ( redAct->numTransRefs > 0 ) {
			/* Write the entry label. */
			out << "\t| " << redAct->actListId+1 << " ->\n";

			/* Write each action in the list of action items. */
			for ( GenActionTable::Iter item = redAct->key; item.lte(); item++ )
				ACTION( out, item->value, 0, false );

			out << "\t()\n";
		}
	}

	genLineDirective( out );
	return out;
}

void OCamlFFlatCodeGen::writeData()
{
	if ( redFsm->anyConditions() ) {
		OPEN_ARRAYS( WIDE_ALPH_TYPE(), CK() );
		COND_KEYS_ARRAYS();
		CLOSE_ARRAYS() <<
		"\n";

		OPEN_ARRAYS( ARRAY_TYPE(redFsm->maxCondSpan), CSP() );
		COND_KEY_SPANS_ARRAYS();
		CLOSE_ARRAYS() <<
		"\n";

		OPEN_ARRAYS( ARRAY_TYPE(redFsm->maxCond), C() );
		CONDS_ARRAYS();
		CLOSE_ARRAYS() <<
		"\n";

		OPEN_ARRAYS( ARRAY_TYPE(redFsm->maxCondIndexOffset), CO() );
		COND_INDEX_OFFSET_ARRAYS();
		CLOSE_ARRAYS() <<
		"\n";
	}

	OPEN_ARRAYS( WIDE_ALPH_TYPE(), K() );
	KEYS_ARRAYS();
	CLOSE_ARRAYS() <<
	"\n";

	OPEN_ARRAYS( ARRAY_TYPE(redFsm->maxSpan), SP() );
	KEY_SPANS_ARRAYS();
	CLOSE_ARRAYS() <<
	"\n";

	OPEN_ARRAYS( ARRAY_TYPE(redFsm->maxFlatIndexOffset), IO() );
	FLAT_INDEX_OFFSET_ARRAYS();
	CLOSE_ARRAYS() <<
	"\n";

	OPEN_ARRAYS( ARRAY_TYPE(redFsm->maxIndex), I() );
	INDICIES_ARRAYS();
	CLOSE_ARRAYS() <<
	"\n";

	OPEN_ARRAYS( ARRAY_TYPE(redFsm->maxState), TT() );
	TRANS_TARGS_ARRAYS();
	CLOSE_ARRAYS() <<
	"\n";

	if ( redFsm->anyActions() ) {
		OPEN_ARRAYS( ARRAY_TYPE(redFsm->maxActListId), TA() );
		TRANS_ACTIONS_ARRAYS();
		CLOSE_ARRAYS() <<
		"\n";
	}

	if ( redFsm->anyToStateActions() ) {
		OPEN_ARRAYS( ARRAY_TYPE(redFsm->maxActionLoc),  TSA() );
		TO_STATE_ACTIONS_ARRAYS();
		CLOSE_ARRAYS() <<
		"\n";
	}

	if ( redFsm->anyFromStateActions() ) {
		OPEN_ARRAYS( ARRAY_TYPE(redFsm->maxActionLoc), FSA() );
		FROM_STATE_ACTIONS_ARRAYS();
		CLOSE_ARRAYS() <<
		"\n";
	}

	if ( redFsm->anyEofActions() ) {
		OPEN_ARRAYS( ARRAY_TYPE(redFsm->maxActListId), EA() );
		EOF_ACTIONS_ARRAYS();
		CLOSE_ARRAYS() <<
		"\n";
	}

	if ( redFsm->anyEofTrans() ) {
		OPEN_ARRAYS( ARRAY_TYPE(redFsm->maxIndexOffset+1), ET() );
		EOF_TRANS_ARRAYS();
		CLOSE_ARRAYS() <<
		"\n";
	}

	STATE_IDS();

  out << "type " << TYPE_STATE() << " = { mutable keys : int; mutable trans : int; }"
    << TOP_SEP();

  out << "exception " << LABEL("match") << TOP_SEP();
  out << "exception " << LABEL("again") << TOP_SEP();
  out << "exception " << LABEL("eof_trans") << TOP_SEP();
}

void OCamlFFlatCodeGen::writeExec()
{
	testEofUsed = false;
	outLabelUsed = false;
	initVarTypes();

	out << 
		"	begin\n";
//		"	" << slenType << " _slen";

//	if ( redFsm->anyRegCurStateRef() )
//		out << ", _ps";
	
//	out << ";\n";
//	out << "	" << transType << " _trans";

//	if ( redFsm->anyConditions() )
//		out << ", _cond";

//	out << ";\n";

//	out <<
//		"	" << "int _keys;\n"
//		"	" << indsType << " _inds;\n";
		/*
		"	" << PTR_CONST() << WIDE_ALPH_TYPE() << POINTER() << "_keys;\n"
		"	" << PTR_CONST() << ARRAY_TYPE(redFsm->maxIndex) << POINTER() << "_inds;\n";*/

  out <<
    "	let state = { keys = 0; trans = 0; } in\n"
    "	let rec do_start () =\n";

//	if ( redFsm->anyConditions() ) {
//		out << 
//			"	" << condsType << " _conds;\n"
//			"	" << WIDE_ALPH_TYPE() << " _widec;\n";
//	}

	if ( !noEnd ) {
		testEofUsed = true;
		out << 
			"	if " << P() << " = " << PE() << " then\n"
			"		do_test_eof ()\n"
      "\telse\n";
	}

	if ( redFsm->errState != 0 ) {
		outLabelUsed = true;
		out << 
			"	if " << vCS() << " = " << redFsm->errState->id << " then\n"
			"		do_out ()\n"
      "\telse\n";
	}
  out << "\tdo_resume ()\n";

	out << "and do_resume () =\n";

	if ( redFsm->anyFromStateActions() ) {
		out <<
			"	begin match " << AT( FSA(), vCS() ) << " with\n";
			FROM_STATE_ACTION_SWITCH();
			SWITCH_DEFAULT() <<
			"	end;\n"
			"\n";
	}

	if ( redFsm->anyConditions() )
		COND_TRANSLATE();

  out << "\tbegin try\n";
	LOCATE_TRANS();
  out << "\twith " << LABEL("match") << " -> () end;\n";

  out << "\tdo_eof_trans ()\n";
	
//	if ( redFsm->anyEofTrans() )
  out << "and do_eof_trans () =\n";

	if ( redFsm->anyRegCurStateRef() )
		out << "	let ps = " << vCS() << " in\n";

	out <<
		"	" << vCS() << " <- " << AT( TT() ,"state.trans" ) << ";\n"
		"\n";

	if ( redFsm->anyRegActions() ) {
		out << 
			"	begin try if " << AT( TA() , "state.trans" ) << " = 0 then\n"
			"		raise " << LABEL("again") << ";\n"
			"\n"
			"	match " << AT( TA(), "state.trans" ) << " with\n";
			ACTION_SWITCH();
			SWITCH_DEFAULT() <<
			"	with " << LABEL("again") << " -> () end;\n"
			"\n";
	}
  out << "\tdo_again ()\n";

//	if ( redFsm->anyRegActions() || redFsm->anyActionGotos() || 
//			redFsm->anyActionCalls() || redFsm->anyActionRets() )
  out << "\tand do_again () =\n";

	if ( redFsm->anyToStateActions() ) {
		out <<
			"	begin match " << AT( TSA(), vCS() ) << " with\n";
			TO_STATE_ACTION_SWITCH();
			SWITCH_DEFAULT() <<
			"	end;\n"
			"\n";
	}

	if ( redFsm->errState != 0 ) {
		outLabelUsed = true;
		out << 
			"	match " << vCS() << " with\n"
      "\t| " << redFsm->errState->id << " -> do_out ()\n"
      "\t| _ ->\n";
	}

  out << "\t" << P() << " <- " << P() << " + 1;\n";

	if ( !noEnd ) {
		out << 
			"	if " << P() << " <> " << PE() << " then\n"
			"		do_resume ()\n"
      "\telse do_test_eof ()\n";
	}
	else {
		out << 
			"	do_resume ()\n";
	}

//	if ( testEofUsed )
	out << "and do_test_eof () =\n";
	
	if ( redFsm->anyEofTrans() || redFsm->anyEofActions() ) {
		out << 
			"	if " << P() << " = " << vEOF() << " then\n"
			"	begin try\n";

		if ( redFsm->anyEofTrans() ) {
			out <<
				"	if " << AT( ET(), vCS() ) << " > 0 then\n"
				"	begin\n"
        "   state.trans <- " << CAST(transType) << "(" << AT( ET(), vCS() ) << " - 1);\n"
				"		" << JUMP("eof_trans") << ";\n"
				"	end;\n";
		}

		if ( redFsm->anyEofActions() ) {
			out <<
				"	begin match " << AT( EA(), vCS() ) << " with\n";
				EOF_ACTION_SWITCH();
				SWITCH_DEFAULT() <<
				"	end\n";
		}

		out << 
			"	with " << LABEL("again") << " -> do_again ()\n"
			"	| " << LABEL("eof_trans") << " -> do_eof_trans () end\n"
			"\n";
	}
  else
  {
    out << "\t()\n";
  }

	if ( outLabelUsed )
		out << "	and do_out () = ()\n";

  out << "\tin do_start ()\n";
	out << "	end;\n";
}

